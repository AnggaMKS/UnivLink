#include "cli.h"
using namespace cli;
namespace {
string singkatPeriode(const string& periode) {
    if (periode.size() >= 10 && periode[4] == '/' && periode[9] == '-' && periode.size() > 10) {
        return periode.substr(2, 2) + periode.substr(7, 2) + periode.substr(10, 1);
    }
    string hasil;
    for (char c : periode) if (c != '/') hasil += c;
    return hasil;
}
}

Admin::Admin(string nomorInduk, string nama, string email, string prodi,const string& password, SistemAkademik* sistem, bool passwordSudahHash) : User(std::move(nomorInduk), std::move(nama), std::move(email), std::move(prodi), password, passwordSudahHash), sistem(sistem) {}

// Ruangan diserahkan ke SistemAkademik. Kode kembar, ditolak dan objek dihapus.
bool Admin::tambahRuangan(unique_ptr<Ruangan> r) {
    if (!sistem || !r) { setPesan("Data ruangan tidak lengkap"); return false; }
    if (r->getKode().empty() || r->getKapasitas() <= 0) { setPesan("Kode wajib diisi dan kapasitas harus lebih dari 0"); return false; }
    if (!sistem->tambahRuangan(std::move(r))) { setPesan("Kode ruangan sudah dipakai"); return false; }
    setPesan("");
    return true;
}

// Membuat kelas baru untuk satu mata kuliah pada satu periode.
// Id <kodeMK>-<huruf A..Z>-<periode ringkas>, mis. TI1101-A-2627G.
Kelas* Admin::buatKelas(MataKuliah& mk, Dosen& dosen, const string& periode, int kuota) {
    if (!sistem) { setPesan("Admin tidak terhubung ke sistem"); return nullptr; }
    if (sistem->cariMataKuliah(mk.getKode()) != &mk) { setPesan("Mata kuliah tidak terdaftar di sistem"); return nullptr; }
    if (sistem->cariUser(dosen.getNomorInduk()) != &dosen) { setPesan("Dosen tidak terdaftar di sistem"); return nullptr; }
    if (periode.empty()) { setPesan("Periode wajib diisi (contoh: 2021/2027-Gasal)"); return nullptr; }
    if (kuota < 1 || kuota > 500) { setPesan("Kuota harus antara 1 dan 500"); return nullptr; }

    string id;
    for (char huruf = 'A'; huruf <= 'Z'; ++huruf) {
        const string kandidat = mk.getKode() + "-" + huruf + "-" + singkatPeriode(periode);
        if (!sistem->cariKelas(kandidat)) { id = kandidat; break; }
    }
    if (id.empty()) { setPesan("Kelas paralel A-Z untuk mata kuliah ini pada periode tersebut sudah penuh"); return nullptr; }

    Kelas* kelas = sistem->tambahKelas(make_unique<Kelas>(id, &mk, &dosen, periode, kuota));
    if (!kelas) { setPesan("Gagal menambah kelas"); return nullptr; }
    dosen.tambahKelasDiampu(kelas);
    setPesan("");
    return kelas;
}

bool Admin::validasiPenempatan(const Kelas& kelas, const Ruangan& r, const Jadwal& j, string& alasan) const {
    const MataKuliah* mk = kelas.getMataKuliah();
    if (!mk) { alasan = "Data kelas tidak lengkap"; return false; }

    if (!j.valid()) {
        alasan = "Jadwal tidak valid (jam 00:00-23:59, jam mulai harus lebih awal dari jam selesai)";
        return false;
    }
    if (r.getJenis() != mk->getJenisRuangan()) {
        alasan = mk->getNama() + " membutuhkan ruang " + jenisRuanganKeString(mk->getJenisRuangan()) +
                 ", bukan " + jenisRuanganKeString(r.getJenis());
        return false;
    }
    if (r.getKapasitas() < kelas.getKuota()) {
        alasan = "Kapasitas ruangan (" + to_string(r.getKapasitas()) + ") kurang dari kuota kelas (" +
                 to_string(kelas.getKuota()) + ")";
        return false;
    }
    if (!r.tersedia(j, kelas.getPeriode())) {
        alasan = "Ruangan " + r.getKode() + " sudah terpakai pada jam tersebut";
        return false;
    }
    if (const Dosen* d = kelas.getDosen()) {
        for (const Kelas* lain : d->getKelasDiampu()) {
            if (lain && lain != &kelas && lain->punyaJadwal() && lain->getPeriode() == kelas.getPeriode() &&
                lain->getJadwal().bentrokDengan(j)) {
                alasan = "Jadwal bentrok dengan kelas " + lain->getId() + " yang diampu dosen yang sama";
                return false;
            }
        }
    }
    // mahasiswa yang sudah terdaftar tidak boleh jadi bentrok akibat perubahan jadwal
    for (const Mahasiswa* m : kelas.getPeserta()) {
        if (!m) continue;
        for (const auto& krs : m->getDaftarKRS()) {
            if (!krs || krs->getPeriode() != kelas.getPeriode()) continue;
            for (const Kelas* lain : krs->getDaftarKelas()) {
                if (lain && lain != &kelas && lain->punyaJadwal() && lain->getJadwal().bentrokDengan(j)) {
                    alasan = "Jadwal baru membuat " + m->getNama() + " bentrok dengan kelas " + lain->getId();
                    return false;
                }
            }
        }
    }
    alasan.clear();
    return true;
}

bool Admin::assignRuangan(Kelas& kelas, Ruangan& r, const Jadwal& j) {
    Ruangan* ruangLama = kelas.getRuangan();
    const Jadwal jadwalLama = kelas.getJadwal();
    const bool punyaLama = kelas.punyaJadwal();

    if (ruangLama) ruangLama->lepas(&kelas);

    string alasan;
    if (!validasiPenempatan(kelas, r, j, alasan)) {
        if (ruangLama && punyaLama) ruangLama->pesan(&kelas);  
        setPesan(alasan);
        return false;
    }

    kelas.setPenempatan(&r, j);
    if (!r.pesan(&kelas)) {
        kelas.setPenempatan(ruangLama, jadwalLama);
        if (ruangLama && punyaLama) ruangLama->pesan(&kelas);
        setPesan("Ruangan menolak pemesanan");
        return false;
    }
    setPesan("");
    return true;
}

//tampilan admin
string Admin::getPeran() const { return "Admin"; }

void Admin::tampilkanDashboard() const {
    cetakHeader();
    judul("DASHBOARD ADMIN");
    if (!sistem) { cout << "  (tidak terhubung ke sistem)\n"; return; }

    int jmlMhs = 0, jmlDosen = 0, jmlAdmin = 0;
    for (const auto& u : sistem->getDaftarUser()) {
        if (dynamic_cast<const Mahasiswa*>(u.get())) ++jmlMhs;
        else if (dynamic_cast<const Dosen*>(u.get())) ++jmlDosen;
        else ++jmlAdmin;
    }
    int tanpaTempat = 0;
    for (const auto& k : sistem->getDaftarKelas()) {
        if (!k->punyaJadwal() || !k->getRuangan()) ++tanpaTempat;
    }
    cout << "  Mahasiswa     : " << jmlMhs << '\n'
         << "  Dosen         : " << jmlDosen << '\n'
         << "  Admin         : " << jmlAdmin << '\n'
         << "  Mata kuliah   : " << sistem->getDaftarMataKuliah().size() << '\n'
         << "  Ruangan       : " << sistem->getDaftarRuangan().size() << '\n'
         << "  Kelas         : " << sistem->getDaftarKelas().size() << '\n';
    if (tanpaTempat > 0) {
        cout << "\n  PERHATIAN: " << tanpaTempat << " kelas belum memiliki ruangan/jadwal "
             << "(belum bisa diambil mahasiswa).\n";
    }
    garis('=');
}

void Admin::tampilkanMenu() const {
    cout << '\n';
    garis('=');
    cout << "  MENU ADMIN\n";
    garis('=');
    cout << "  1. Lihat Dashboard\n";
    cout << "  2. Kelola Pengguna (Dosen / Mahasiswa / Admin)\n";
    cout << "  3. Kelola Mata Kuliah & Kurikulum\n";
    cout << "  4. Kelola Ruangan\n";
    cout << "  5. Kelola Kelas (buat kelas, tempatkan ruangan & jadwal)\n";
    cout << "  6. Ganti Kata Sandi\n";
    cout << "  0. Logout\n";
    garis('-');
    cout << "  Pilihan Anda: ";
}
