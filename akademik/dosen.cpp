#include "cli/cli.h"
#include <algorithm>
using namespace cli;
namespace {
bool komponenOtomatis(const string& nama) { return nama == "tugas" || nama == "kehadiran"; }

// Mencari id yang belum dipakai: prefix + n, n dimulai dari (jumlah + 1).
template <typename Daftar, typename AmbilId>
string idBaru(const string& prefix, const Daftar& daftar, AmbilId ambilId) {
    size_t n = daftar.size() + 1;
    for (;; ++n) {
        const string kandidat = prefix + to_string(n);
        bool dipakai = false;
        for (const auto* x : daftar) {
            if (ambilId(x) == kandidat) { dipakai = true; break; }
        }
        if (!dipakai) return kandidat;
    }
}

}

void Dosen::tambahKelasDiampu(Kelas* kelas) {
    if (!kelas) return;
    if (find(kelasDiampu.begin(), kelasDiampu.end(), kelas) != kelasDiampu.end()) return;
    kelasDiampu.push_back(kelas);
}

vector<Kelas*> Dosen::getKelasDiampu() const { return kelasDiampu; }

bool Dosen::mengampu(const Kelas& kelas) const { return kelas.getDosen() == this; }

// pengumuman
Pengumuman* Dosen::buatPengumuman(Kelas& kelas, const string& judul, const string& isi) {
    if (!mengampu(kelas)) { setPesan("Anda bukan dosen pengampu kelas ini"); return nullptr; }
    if (judul.empty() || isi.empty()) { setPesan("Judul dan isi pengumuman tidak boleh kosong"); return nullptr; }

    const string id = idBaru("P-" + kelas.getId() + "-", kelas.getDaftarPengumuman(),
                             [](const Pengumuman* p) { return p->getId(); });
    auto p = make_unique<Pengumuman>(id, judul, isi, this);
    Pengumuman* mentah = p.get();
    kelas.tambahPengumuman(std::move(p)); 
    setPesan("");
    return mentah;  // status DRAFT sampai publikasikanPengumuman()
}

bool Dosen::editPengumuman(Pengumuman& p, const string& isiBaru) {
    if (p.getPembuat() != this) { setPesan("Hanya pembuat pengumuman yang boleh mengubahnya"); return false; }
    if (!p.edit(isiBaru)) { setPesan("Isi pengumuman tidak boleh kosong"); return false; }
    setPesan("");
    return true;
}

bool Dosen::publikasikanPengumuman(Pengumuman& p) {
    if (p.getPembuat() != this) { setPesan("Hanya pembuat pengumuman yang boleh menerbitkannya"); return false; }
    if (p.getStatus() == StatusPengumuman::TERBIT) { setPesan("Pengumuman sudah terbit"); return false; }
    p.publikasikan();
    setPesan("");
    return true;
}

// tugas
Tugas* Dosen::buatTugas(Kelas& kelas, const string& judul, const string& deskripsi,
                        time_t deadline, double bobot) {
    if (!mengampu(kelas)) { setPesan("Anda bukan dosen pengampu kelas ini"); return nullptr; }
    if (judul.empty()) { setPesan("Judul tugas tidak boleh kosong"); return nullptr; }
    if (deadline <= time(nullptr)) { setPesan("Tenggat harus di masa depan"); return nullptr; }
    if (!(bobot > 0.0 && bobot <= 100.0)) { setPesan("Bobot tugas harus lebih dari 0 dan paling besar 100"); return nullptr; }

    const string id = idBaru("T-" + kelas.getId() + "-", kelas.getDaftarTugas(),
                             [](const Tugas* t) { return t->getId(); });
    auto t = make_unique<Tugas>(id, judul, deskripsi, deadline, bobot);
    Tugas* mentah = t.get();
    kelas.tambahTugas(std::move(t));
    setPesan("");
    return mentah;
}

// Memberi skor 0..100 pada pengumpulan seorang mahasiswa.
bool Dosen::beriSkorTugas(Tugas& tugas, Mahasiswa& mhs, double skor) {
    bool milikSaya = false;
    for (const Kelas* k : kelasDiampu) {
        if (k && k->getDosen() == this && k->memilikiTugas(&tugas)) { milikSaya = true; break; }
    }
    if (!milikSaya) { setPesan("Tugas ini bukan milik kelas yang Anda ampu"); return false; }

    Pengumpulan* p = tugas.getPengumpulan(mhs);
    if (!p) { setPesan(mhs.getNama() + " belum mengumpulkan tugas ini"); return false; }
    if (!p->beriSkor(skor)) { setPesan("Skor harus antara 0 dan 100"); return false; }
    setPesan("");
    return true;
}

// nilai dan kehadiran
bool Dosen::inputNilai(Kelas& kelas, Mahasiswa& mhs, const string& komponen, double skor) {
    if (!mengampu(kelas)) { setPesan("Anda bukan dosen pengampu kelas ini"); return false; }
    if (!kelas.memilikiPeserta(mhs)) { setPesan(mhs.getNama() + " bukan peserta kelas ini"); return false; }

    const string nama = normalisasi(komponen);
    const MataKuliah* mk = kelas.getMataKuliah();
    if (!mk || mk->getBobotKomponen().count(nama) == 0) {
        setPesan("Komponen '" + komponen + "' tidak ada pada mata kuliah ini");
        return false;
    }
    if (komponenOtomatis(nama)) {
        setPesan("Komponen '" + nama + "' dihitung otomatis dari tugas/presensi saat finalisasi");
        return false;
    }
    if (!(skor >= 0.0 && skor <= 100.0)) { setPesan("Skor harus antara 0 dan 100"); return false; }

    Nilai* n = mhs.getTranskrip().ambilAtauBuatNilai(&kelas);
    if (!n || n->isFinal()) { setPesan("Nilai sudah final dan tidak dapat diubah"); return false; }
    if (!n->updateKomponen(nama, skor)) { setPesan("Komponen tidak dapat diperbarui"); return false; }
    setPesan("");
    return true;
}

bool Dosen::updateKehadiran(Kelas& kelas, Mahasiswa& mhs, int pertemuanKe, bool hadir) {
    if (!mengampu(kelas)) { setPesan("Anda bukan dosen pengampu kelas ini"); return false; }
    if (pertemuanKe < 1 || pertemuanKe > kelas.getTotalPertemuan()) {
        setPesan("Pertemuan harus antara 1 dan " + to_string(kelas.getTotalPertemuan()));
        return false;
    }
    if (!kelas.catatKehadiran(mhs, pertemuanKe, hadir)) {
        setPesan(mhs.getNama() + " bukan peserta kelas ini");
        return false;
    }
    setPesan("");
    return true;
}

// Finalisasi untuk SELURUH peserta yang nilainya belum final:
//   1. isi komponen otomatis ("tugas" dari Kelas::hitungNilaiTugas,
//      "kehadiran" dari Kelas::hitungPersentaseKehadiran)
//   2. Nilai::finalisasi() -> skor akhir, huruf, bobot, lalu terkunci
// Komponen yang belum diinput dosen dihitung 0

bool Dosen::finalisasiNilai(Kelas& kelas) {
    if (!mengampu(kelas)) { setPesan("Anda bukan dosen pengampu kelas ini"); return false; }
    if (!kelas.getMataKuliah()) { setPesan("Data kelas tidak lengkap"); return false; }

    int diproses = 0;
    for (Mahasiswa* m : kelas.getPeserta()) {
        if (!m) continue;
        Nilai* n = m->getTranskrip().ambilAtauBuatNilai(&kelas);
        if (!n || n->isFinal()) continue;

        n->updateKomponen("tugas", kelas.hitungNilaiTugas(*m));            
        n->updateKomponen("kehadiran", kelas.hitungPersentaseKehadiran(*m));
        if (n->finalisasi()) ++diproses;
    }
    if (diproses == 0) { setPesan("Tidak ada nilai yang perlu difinalisasi"); return false; }
    setPesan("");
    return true;
}

string Dosen::getPeran() const { return "Dosen"; }

void Dosen::tampilkanDashboard() const {
    cetakHeader();
    judul("DASHBOARD DOSEN");
    cout << "  Email         : " << getEmail() << "\n\n";
    garis();
    cout << "  KELAS YANG DIAMPU\n";
    garis();
    if (kelasDiampu.empty()) {
        cout << "  (Belum mengampu kelas)\n";
    }
    for (const Kelas* k : kelasDiampu) {
        if (!k || !k->getMataKuliah()) continue;
        cout << "  [" << k->getId() << "] " << k->getMataKuliah()->getNama()
             << "  (" << k->getMataKuliah()->getSKS() << " SKS)\n"
             << "      Periode : " << k->getPeriode() << "   Peserta : " << k->jumlahPeserta() << "/" << k->getKuota() << '\n'
             << "      Jadwal  : " << (k->punyaJadwal() ? k->getJadwal().toString() : "-")
             << "   Ruangan : " << (k->getRuangan() ? k->getRuangan()->getKode() : "-") << '\n';
    }
    garis('=');
}

void Dosen::tampilkanMenu() const {
    cout << '\n';
    garis('=');
    cout << "  MENU DOSEN\n";
    garis('=');
    cout << "  1. Lihat Dashboard\n";
    cout << "  2. Pengumuman Kelas\n";
    cout << "  3. Manajemen Penugasan\n";
    cout << "  4. Manajemen Penilaian\n";
    cout << "  5. Manajemen Kehadiran\n";
    cout << "  6. Ganti Kata Sandi\n";
    cout << "  0. Logout\n";
    garis('-');
    cout << "  Pilihan Anda: ";
}
