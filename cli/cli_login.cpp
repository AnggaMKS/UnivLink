#include "cli.h"
#include <ctime>
#include <set>
using namespace cli;
namespace cli {

// Pendaftaran mandiri mahasiswa. Prodi dipilih dari kurikulum yang ada, sehingga setiap mahasiswa pasti punya kurikulum (dibutuhkan untuk registrasi KRS).
bool registrasiMahasiswa(SistemAkademik& sistem) {
    judul("DAFTAR AKUN MAHASISWA");
    vector<string> prodi;
    {
        set<string> unik;
        for (const auto& k : sistem.getDaftarKurikulum()) unik.insert(k->getProdi());
        prodi.assign(unik.begin(), unik.end());
    }
    if (prodi.empty()) { cout << "  ! Belum ada kurikulum. Hubungi Admin.\n"; return false; }

    const string niu = bacaBaris("  NIU (kosong = batal): ");
    if (niu.empty()) return false;
    if (niu.find('|') != string::npos || niu.find(' ') != string::npos) { cout << "  ! NIU tidak boleh mengandung spasi atau '|'.\n"; return false; }
    if (sistem.cariUser(niu)) { cout << "  ! NIU sudah terdaftar.\n"; return false; }
    const string nama = bacaTidakKosong("  Nama lengkap : ");
    const string email = bacaTidakKosong("  Email        : ");
    cout << "  Program studi:\n";
    for (size_t i = 0; i < prodi.size(); ++i) cout << "   " << (i + 1) << ". " << prodi[i] << '\n';
    const int p = pilihNomor("  Pilih prodi", static_cast<int>(prodi.size()));
    if (p < 0) return false;
    Kurikulum* kur = sistem.getKurikulum(prodi[p]);
    if (!kur) { cout << "  ! Kurikulum prodi tidak ditemukan.\n"; return false; }

    const time_t sekarang = time(nullptr);
    const int tahunIni = localtime(&sekarang)->tm_year + 1900;
    const int angkatan = bacaInt("  Angkatan (tahun masuk): ", 2000, tahunIni);
    const int semester = bacaInt("  Semester berjalan (1-14): ", 1, 14);
    const string pw = bacaPassword("  Kata sandi (min. 6 karakter): ");
    if (pw.size() < 6) { cout << "  ! Kata sandi minimal 6 karakter.\n"; return false; }
    const string ulang = bacaPassword("  Ulangi kata sandi: ");
    if (pw != ulang) { cout << "  ! Kata sandi tidak sama.\n"; return false; }

    if (!sistem.tambahUser(make_unique<Mahasiswa>(niu, nama, email, prodi[p], pw, angkatan, semester, kur))) {
        cout << "  ! Gagal membuat akun.\n";
        return false;
    }
    cout << "  Akun dibuat. Silakan masuk dengan NIU Anda.\n";
    return true;
}

} 

namespace {

void layarMasuk(SistemAkademik& sistem) {
    judul("MASUK");
    for (int percobaan = 1; percobaan <= 3; ++percobaan) {
        const string niu = bacaBaris("  NIU/NIP (kosong = batal): ");
        if (niu.empty()) return;
        const string pw = bacaPassword("  Kata sandi: ");
        User* u = sistem.login(niu, pw);
        if (!u) { cout << "  ! NIU atau kata sandi salah (" << percobaan << "/3).\n"; continue; }

        cout << "\n  Selamat datang, " << u->getNama() << " (" << u->getPeran() << ")\n";
        if (auto* m = dynamic_cast<Mahasiswa*>(u)) menuMahasiswa(sistem, *m);
        else if (auto* d = dynamic_cast<Dosen*>(u)) menuDosen(sistem, *d);
        else if (auto* a = dynamic_cast<Admin*>(u)) menuAdmin(sistem, *a);
        sistem.logout();
        if (!sistem.simpanData()) cout << "  ! PERINGATAN: data gagal disimpan.\n";
        cout << "\n  Anda telah keluar.\n";
        return;
    }
    cout << "  Terlalu banyak percobaan gagal.\n";
}

}

void jalankanAplikasi(SistemAkademik& sistem) {
    for (;;) {
        cout << '\n';
        judul("UNIVLINK Teknologi Informasi - Sistem Informasi Akademik");
        cout << "  1. Masuk\n  2. Daftar akun mahasiswa\n  0. Keluar\n";
        garis('-');
        const int p = bacaInt("  Pilihan Anda: ", 0, 2);
        if (p == 0) return;
        if (p == 1) layarMasuk(sistem);
        else if (registrasiMahasiswa(sistem)) sistem.simpanData();
    }
}
