#include "cli.h"
#include <algorithm>
#include <iomanip>
using namespace cli;
namespace {

void laporGagal(const Admin& a) {
    cout << "  ! Ditolak: " << (a.getPesanTerakhir().empty() ? "operasi gagal" : a.getPesanTerakhir()) << '\n';
}

// user
void daftarPengguna(const SistemAkademik& sistem) {
    judul("DAFTAR PENGGUNA");
    cout << "  " << left << setw(14) << "NIU/NIP" << setw(26) << "Nama" << setw(11) << "Peran" << setw(24) << "Prodi" << "Email\n";
    garis('-', 90);
    for (const auto& u : sistem.getDaftarUser()) {
        cout << "  " << left << setw(14) << u->getNomorInduk() << setw(26) << pangkas(u->getNama(), 25)
             << setw(11) << u->getPeran() << setw(24) << pangkas(u->getProdi(), 23) << u->getEmail() << '\n';
    }
}

void tambahPengguna(SistemAkademik& sistem, Admin& admin) {
    cout << "  1. Mahasiswa\n  2. Dosen\n  3. Admin\n  0. Batal\n";
    const int jenis = bacaInt("  Jenis pengguna: ", 0, 3);
    if (jenis == 0) return;
    if (jenis == 1) {
        if (registrasiMahasiswa(sistem)) sistem.simpanData();
        return;
    }
    const string niu = bacaBaris("  NIP/ID (kosong = batal): ");
    if (niu.empty()) return;
    if (sistem.cariUser(niu)) { cout << "  ! ID sudah dipakai.\n"; return; }
    const string nama = bacaTidakKosong("  Nama     : ");
    const string email = bacaTidakKosong("  Email    : ");
    const string prodi = bacaTidakKosong("  Prodi    : ");
    const string pw = bacaPassword("  Kata sandi awal (min. 6 karakter): ");
    if (pw.size() < 6) { cout << "  ! Kata sandi minimal 6 karakter.\n"; return; }
    unique_ptr<User> baru;
    if (jenis == 2) baru = make_unique<Dosen>(niu, nama, email, prodi, pw);
    else baru = make_unique<Admin>(niu, nama, email, prodi, pw, &sistem);
    if (sistem.tambahUser(std::move(baru))) cout << "  Pengguna ditambahkan.\n";
    else cout << "  ! Gagal menambahkan pengguna.\n";
    (void)admin;
}

void layarPengguna(SistemAkademik& sistem, Admin& admin) {
    for (;;) {
        judul("KELOLA PENGGUNA");
        cout << "  1. Lihat semua pengguna\n  2. Tambah pengguna\n  0. Kembali\n";
        const int p = bacaInt("  Pilihan: ", 0, 2);
        if (p == 0) return;
        if (p == 1) daftarPengguna(sistem);
        else tambahPengguna(sistem, admin);
        tunggu();
    }
}

// matkul dan kurikulum
vector<MataKuliah*> mkUrut(const SistemAkademik& sistem) {
    vector<MataKuliah*> v;
    for (const auto& m : sistem.getDaftarMataKuliah()) v.push_back(m.get());
    sort(v.begin(), v.end(), [](const MataKuliah* a, const MataKuliah* b) {
        if (a->getSemester() != b->getSemester()) return a->getSemester() < b->getSemester();
        return a->getKode() < b->getKode();
    });
    return v;
}

void tabelMK(const vector<MataKuliah*>& v) {
    cout << "  " << left << setw(4) << "No" << setw(10) << "Kode" << setw(30) << "Nama" << setw(5) << "SKS" << setw(5) << "Smt"
         << setw(11) << "Jenis" << "Prasyarat\n";
    garis('-', 90);
    for (size_t i = 0; i < v.size(); ++i) {
        string pra;
        for (const MataKuliah* p : v[i]->getPrasyarat()) pra += (pra.empty() ? "" : ", ") + p->getKode();
        cout << "  " << left << setw(4) << (i + 1) << setw(10) << v[i]->getKode() << setw(30) << pangkas(v[i]->getNama(), 29)
             << setw(5) << v[i]->getSKS() << setw(5) << v[i]->getSemester() << setw(11) << jenisKeString(*v[i])
             << (pra.empty() ? "-" : pra) << '\n';
    }
}

void tambahMK(SistemAkademik& sistem) {
    const string kode = bacaBaris("  Kode MK (kosong = batal): ");
    if (kode.empty()) return;
    if (sistem.cariMataKuliah(kode)) { cout << "  ! Kode sudah dipakai.\n"; return; }
    const string nama = bacaTidakKosong("  Nama        : ");
    const int sks = bacaInt("  SKS (1-6)   : ", 1, 6);
    const int smt = bacaInt("  Semester (1-8): ", 1, 8);
    cout << "  1. Teori (ruang kelas)\n  2. Praktikum (laboratorium)\n";
    const int jenis = bacaInt("  Jenis: ", 1, 2);
    unique_ptr<MataKuliah> mk;
    if (jenis == 1) mk = make_unique<MataKuliahTeori>(kode, nama, sks, smt);
    else mk = make_unique<MataKuliahPraktikum>(kode, nama, sks, smt);
    if (sistem.tambahMataKuliah(std::move(mk))) cout << "  Mata kuliah ditambahkan. Masukkan ke kurikulum lewat menu 'Kurikulum'.\n";
    else cout << "  ! Gagal menambahkan mata kuliah.\n";
}

void ubahPrasyarat(SistemAkademik& sistem) {
    const vector<MataKuliah*> v = mkUrut(sistem);
    if (v.size() < 2) { cout << "  Perlu minimal dua mata kuliah.\n"; return; }
    tabelMK(v);
    const int a = pilihNomor("\n  Mata kuliah yang memiliki prasyarat", static_cast<int>(v.size()));
    if (a < 0) return;
    const int b = pilihNomor("  Mata kuliah yang menjadi prasyarat", static_cast<int>(v.size()));
    if (b < 0) return;
    if (v[a]->tambahPrasyarat(v[b])) cout << "  " << v[b]->getKode() << " kini prasyarat " << v[a]->getKode() << ".\n";
    else cout << "  ! Ditolak (prasyarat sudah ada, diri sendiri, atau membentuk siklus).\n";
}

void ubahBobot(SistemAkademik& sistem) {
    const vector<MataKuliah*> v = mkUrut(sistem);
    if (v.empty()) return;
    tabelMK(v);
    const int i = pilihNomor("\n  Mata kuliah", static_cast<int>(v.size()));
    if (i < 0) return;
    MataKuliah* mk = v[i];
    cout << "  Bobot sekarang:";
    for (const auto& [nama, b] : mk->getBobotKomponen()) cout << ' ' << nama << '=' << b;
    cout << "\n  Masukkan bobot baru. Komponen \"tugas\" dan \"kehadiran\" dihitung otomatis; total harus 100.\n" << "  Format: nama=bobot dipisah spasi, contoh: tugas=20 uts=30 uas=40 kehadiran=10\n";
    const string baris = bacaBaris("  > ");
    if (baris.empty()) return;
    map<string, double> baru;
    size_t pos = 0;

    while (pos < baris.size()) {
        size_t akhir = baris.find(' ', pos);
        if (akhir == string::npos) akhir = baris.size();
        const string token = baris.substr(pos, akhir - pos);
        pos = akhir + 1;
        if (token.empty()) continue;
        const size_t sama = token.find('=');
        if (sama == string::npos || sama == 0) { cout << "  ! Format salah pada \"" << token << "\".\n"; return; }
        char* ujung = nullptr;
        const double nilai = strtod(token.c_str() + sama + 1, &ujung);
        if (*ujung != '\0') { cout << "  ! Bobot bukan angka pada \"" << token << "\".\n"; return; }
        baru[normalisasi(token.substr(0, sama))] = nilai;
    }
    if (mk->setBobotKomponen(baru)) cout << "  Bobot diperbarui.\n";
    else cout << "  ! Ditolak: total bobot harus 100 dan tidak boleh negatif.\n";
}

void kelolaKurikulum(SistemAkademik& sistem) {
    judul("KURIKULUM");
    for (const auto& k : sistem.getDaftarKurikulum()) {
        cout << "  " << k->getProdi() << " (" << k->getTahun() << ") - " << k->semuaMataKuliah().size()
             << " mata kuliah, graf prasyarat " << (k->validasiGrafPrasyarat() ? "valid" : "TIDAK valid") << '\n';
    }
    cout << "\n  1. Buat kurikulum baru\n  2. Masukkan mata kuliah ke kurikulum\n  3. Lihat isi kurikulum\n  0. Kembali\n";
    const int p = bacaInt("  Pilihan: ", 0, 3);
    if (p == 0) return;
    if (p == 1) {
        const string prodi = bacaBaris("  Nama prodi (kosong = batal): ");
        if (prodi.empty()) return;
        const int tahun = bacaInt("  Tahun kurikulum: ", 2000, 2100);
        if (sistem.tambahKurikulum(make_unique<Kurikulum>(prodi, tahun))) cout << "  Kurikulum dibuat.\n";
        else cout << "  ! Kurikulum prodi/tahun tersebut sudah ada.\n";
        return;
    }
    vector<Kurikulum*> daftar;
    for (const auto& k : sistem.getDaftarKurikulum()) daftar.push_back(k.get());
    if (daftar.empty()) { cout << "  Belum ada kurikulum.\n"; return; }
    for (size_t i = 0; i < daftar.size(); ++i) cout << "   " << (i + 1) << ". " << daftar[i]->getProdi() << " (" << daftar[i]->getTahun() << ")\n";
    const int idx = pilihNomor("  Pilih kurikulum", static_cast<int>(daftar.size()));
    if (idx < 0) return;
    Kurikulum* kur = daftar[idx];
    if (p == 3) {
        tabelMK(kur->semuaMataKuliah());
        return;
    }
    const vector<MataKuliah*> v = mkUrut(sistem);
    tabelMK(v);
    const int m = pilihNomor("\n  Mata kuliah yang dimasukkan", static_cast<int>(v.size()));
    if (m < 0) return;
    if (kur->tambahMataKuliah(v[m])) {
        cout << "  Ditambahkan ke kurikulum.\n";
        if (!kur->validasiGrafPrasyarat()) cout << "  PERHATIAN: ada prasyarat yang belum masuk kurikulum ini.\n";
    } else {
        cout << "  ! Ditolak (sudah ada di kurikulum).\n";
    }
}

void layarMataKuliah(SistemAkademik& sistem) {
    for (;;) {
        judul("MATA KULIAH & KURIKULUM");
        cout << "  1. Lihat mata kuliah\n  2. Tambah mata kuliah\n  3. Tambah prasyarat\n  4. Ubah bobot penilaian\n"
             << "  5. Kurikulum\n  0. Kembali\n";
        const int p = bacaInt("  Pilihan: ", 0, 5);
        if (p == 0) return;
        switch (p) {
            case 1: tabelMK(mkUrut(sistem)); break;
            case 2: tambahMK(sistem); break;
            case 3: ubahPrasyarat(sistem); break;
            case 4: ubahBobot(sistem); break;
            case 5: kelolaKurikulum(sistem); break;
        }
        tunggu();
    }
}

// ruangan
void layarRuangan(SistemAkademik& sistem, Admin& admin) {
    for (;;) {
        judul("KELOLA RUANGAN");
        cout << "  " << left << setw(10) << "Kode" << setw(18) << "Gedung" << setw(11) << "Kapasitas" << "Jenis\n";
        garis('-', 50);
        for (const auto& r : sistem.getDaftarRuangan()) {
            cout << "  " << left << setw(10) << r->getKode() << setw(18) << pangkas(r->getGedung(), 17) << setw(11) << r->getKapasitas()
                 << jenisRuanganKeString(r->getJenis()) << '\n';
        }
        cout << "\n  1. Tambah ruangan\n  0. Kembali\n";
        if (bacaInt("  Pilihan: ", 0, 1) == 0) return;
        const string kode = bacaBaris("  Kode ruangan (kosong = batal): ");
        if (kode.empty()) continue;
        const string gedung = bacaTidakKosong("  Gedung   : ");
        const int kap = bacaInt("  Kapasitas (1-1000): ", 1, 1000);
        cout << "  1. Ruang kelas\n  2. Laboratorium\n";
        const JenisRuangan jenis = bacaInt("  Jenis: ", 1, 2) == 1 ? JenisRuangan::KELAS : JenisRuangan::LAB;
        if (admin.tambahRuangan(make_unique<Ruangan>(kode, gedung, kap, jenis))) cout << "  Ruangan ditambahkan.\n";
        else laporGagal(admin);
        tunggu();
    }
}

// kelas
vector<Kelas*> kelasUrut(const SistemAkademik& sistem) {
    vector<Kelas*> v;
    for (const auto& k : sistem.getDaftarKelas()) v.push_back(k.get());
    sort(v.begin(), v.end(), [](const Kelas* a, const Kelas* b) {
        if (a->getPeriode() != b->getPeriode()) return a->getPeriode() > b->getPeriode();
        return a->getId() < b->getId();
    });
    return v;
}

void buatKelasBaru(SistemAkademik& sistem, Admin& admin) {
    const vector<MataKuliah*> mks = mkUrut(sistem);
    vector<Dosen*> dosen;
    for (const auto& u : sistem.getDaftarUser()) if (auto* d = dynamic_cast<Dosen*>(u.get())) dosen.push_back(d);
    if (mks.empty() || dosen.empty()) { cout << "  ! Perlu minimal satu mata kuliah dan satu dosen.\n"; return; }

    tabelMK(mks);
    const int m = pilihNomor("\n  Mata kuliah", static_cast<int>(mks.size()));
    if (m < 0) return;
    cout << '\n';
    for (size_t i = 0; i < dosen.size(); ++i) cout << "   " << (i + 1) << ". " << dosen[i]->getNama() << " (" << dosen[i]->getNomorInduk() << ")\n";
    const int d = pilihNomor("  Dosen pengampu", static_cast<int>(dosen.size()));
    if (d < 0) return;
    const string periode = bacaPeriode("  Periode", periodeTerbaru(sistem));
    const int kuota = bacaInt("  Kuota (1-500): ", 1, 500);
    Kelas* k = admin.buatKelas(*mks[m], *dosen[d], periode, kuota);
    if (k) cout << "  Kelas dibuat: " << k->getId() << ". Jangan lupa tempatkan ruangan & jadwal.\n";
    else laporGagal(admin);
}

void tempatkanKelas(SistemAkademik& sistem, Admin& admin) {
    const vector<Kelas*> kelas = kelasUrut(sistem);
    if (kelas.empty()) { cout << "  Belum ada kelas.\n"; return; }
    cetakDaftarKelas(kelas, true);
    const int k = pilihNomor("\n  Pilih kelas", static_cast<int>(kelas.size()));
    if (k < 0) return;

    const vector<Ruangan*> ruang = [&] {
        vector<Ruangan*> r;
        for (const auto& x : sistem.getDaftarRuangan()) r.push_back(x.get());
        return r;
    }();
    if (ruang.empty()) { cout << "  ! Belum ada ruangan. Tambahkan dulu.\n"; return; }
    cout << "\n  Kebutuhan: ruang " << jenisRuanganKeString(kelas[k]->getMataKuliah()->getJenisRuangan())
         << ", kapasitas >= " << kelas[k]->getKuota() << '\n';
    for (size_t i = 0; i < ruang.size(); ++i)
        cout << "   " << (i + 1) << ". " << ruang[i]->getKode() << "  " << ruang[i]->getGedung() << "  kap " << ruang[i]->getKapasitas()
             << "  " << jenisRuanganKeString(ruang[i]->getJenis()) << '\n';
    const int r = pilihNomor("  Pilih ruangan", static_cast<int>(ruang.size()));
    if (r < 0) return;

    cout << "  Hari: 1 Senin  2 Selasa  3 Rabu  4 Kamis  5 Jumat\n";
    const Hari hari = static_cast<Hari>(bacaInt("  Hari: ", 1, 5) - 1);
    const int mulai = bacaJam("  Jam mulai");
    const int selesai = bacaJam("  Jam selesai");
    if (admin.assignRuangan(*kelas[k], *ruang[r], Jadwal(hari, mulai, selesai)))
        cout << "  Berhasil: " << kelas[k]->getId() << " -> " << ruang[r]->getKode() << ", " << kelas[k]->getJadwal().toString() << '\n';
    else laporGagal(admin);
}

void detailKelas(SistemAkademik& sistem) {
    const vector<Kelas*> kelas = kelasUrut(sistem);
    if (kelas.empty()) { cout << "  Belum ada kelas.\n"; return; }
    cetakDaftarKelas(kelas, true);
    const int k = pilihNomor("\n  Pilih kelas", static_cast<int>(kelas.size()));
    if (k < 0) return;
    const Kelas& x = *kelas[k];
    cout << "\n  Kelas   : " << x.getId() << "  (" << x.getMataKuliah()->getNama() << ", " << x.getMataKuliah()->getSKS() << " SKS)\n"
         << "  Periode : " << x.getPeriode() << "\n  Dosen   : " << (x.getDosen() ? x.getDosen()->getNama() : "-") << '\n'
         << "  Tempat  : " << (x.getRuangan() ? x.getRuangan()->getKode() : "-") << ", " << (x.punyaJadwal() ? x.getJadwal().toString() : "-") << '\n'
         << "  Peserta : " << x.jumlahPeserta() << " / " << x.getKuota() << '\n';
    for (const Mahasiswa* m : x.getPeserta()) cout << "     - " << m->getNomorInduk() << "  " << m->getNama() << '\n';
}

void layarKelas(SistemAkademik& sistem, Admin& admin) {
    for (;;) {
        judul("KELOLA KELAS");
        cout << "  1. Daftar kelas\n  2. Buat kelas baru\n  3. Tempatkan ruangan & jadwal\n  4. Detail kelas\n  0. Kembali\n";
        const int p = bacaInt("  Pilihan: ", 0, 4);
        if (p == 0) return;
        switch (p) {
            case 1: { const vector<Kelas*> v = kelasUrut(sistem); if (v.empty()) cout << "  (belum ada kelas)\n"; else cetakDaftarKelas(v, true); break; }
            case 2: buatKelasBaru(sistem, admin); break;
            case 3: tempatkanKelas(sistem, admin); break;
            case 4: detailKelas(sistem); break;
        }
        tunggu();
    }
}

}

void menuAdmin(SistemAkademik& sistem, Admin& admin) {
    admin.tampilkanDashboard();
    for (;;) {
        admin.tampilkanMenu();
        const int pilihan = bacaInt("", 0, 6);
        switch (pilihan) {
            case 1: admin.tampilkanDashboard(); tunggu(); break;
            case 2: layarPengguna(sistem, admin); break;
            case 3: layarMataKuliah(sistem); break;
            case 4: layarRuangan(sistem, admin); break;
            case 5: layarKelas(sistem, admin); break;
            case 6: layarGantiPassword(admin); tunggu(); break;
            case 0: return;
        }
        if (!sistem.simpanData()) cout << "  ! PERINGATAN: data gagal disimpan.\n";
    }
}
