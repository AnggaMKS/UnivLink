// Fungsi bantu antarmuka CLI: tampilan, pembacaan input yang aman, dan beberapa
// bantu kecil. Semua input dibaca per BARIS dengan getline lalu di-parse sendiri.
// Tidak memakai `cin >> x` karena itu meninggalkan '\n' di buffer dan merusak
// pembacaan berikutnya (penyebab klasik "Enter harus ditekan dua kali").
// Bila input habis (EOF), semua pembaca melempar InputBerakhir supaya program
// berhenti rapi, bukan berputar selamanya.

#include "cli.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iomanip>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace cli {
void garis(char c, int n) { cout << string(n, c) << '\n'; }
void judul(const string& teks) {
    garis('=');
    cout << "  " << teks << '\n';
    garis('=');
}

string pangkas(const string& teks, size_t maks) {
    if (teks.size() <= maks) return teks;
    if (maks <= 3) return teks.substr(0, maks);
    return teks.substr(0, maks - 3) + "...";
}

string jenisKeString(const MataKuliah& mk) {
    return dynamic_cast<const MataKuliahPraktikum*>(&mk) ? "Praktikum" : "Teori";
}

// Sebaran nilai satu kelas
string ringkasDistribusi(const Kelas& kelas) {
    const map<NilaiHuruf, int> dist = kelas.hitungDistribusiNilai();
    string hasil;
    int total = 0;
    for (const auto& [huruf, jumlah] : dist) {
        if (jumlah <= 0) continue;
        if (!hasil.empty()) hasil += ' ';
        hasil += KonversiNilai::hurufKeString(huruf) + ":" + to_string(jumlah);
        total += jumlah;
    }
    return total == 0 ? "-" : hasil + " (n=" + to_string(total) + ")";
}

// input
namespace {
string pangkasSpasi(const string& s) {
    size_t a = 0, b = s.size();
    while (a < b && isspace(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return s.substr(a, b - a);
}

string ambilBaris() {
    string baris;
    if (!getline(cin, baris)) throw InputBerakhir();
    if (!baris.empty() && baris.back() == '\r') baris.pop_back();  // berkas input gaya Windows
    return baris;
}

} 

string bacaBaris(const string& prompt) {
    cout << prompt << flush;
    return pangkasSpasi(ambilBaris());
}

string bacaTidakKosong(const string& prompt) {
    for (;;) {
        string s = bacaBaris(prompt);
        if (!s.empty()) return s;
        cout << "  ! Tidak boleh kosong.\n";
    }
}

int bacaInt(const string& prompt, int minimal, int maksimal) {
    bool pertama = true;
    for (;;) {
        cout << (pertama ? prompt : string("  > ")) << flush;
        pertama = false;
        const string s = pangkasSpasi(ambilBaris());
        char* akhir = nullptr;
        const long v = strtol(s.c_str(), &akhir, 10);
        if (!s.empty() && *akhir == '\0' && v >= minimal && v <= maksimal) return static_cast<int>(v);
        cout << "  ! Masukkan angka bulat " << minimal << " sampai " << maksimal << ".\n";
    }
}

double bacaDouble(const string& prompt, double minimal, double maksimal) {
    for (;;) {
        string s = bacaBaris(prompt);
        replace(s.begin(), s.end(), ',', '.');  // "85,5" juga diterima
        char* akhir = nullptr;
        const double v = strtod(s.c_str(), &akhir);
        if (!s.empty() && *akhir == '\0' && v >= minimal && v <= maksimal) return v;
        cout << "  ! Masukkan angka " << minimal << " sampai " << maksimal << ".\n";
    }
}

bool konfirmasi(const string& prompt) {
    for (;;) {
        string s = bacaBaris(prompt + " (y/t): ");
        transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return tolower(c); });
        if (s == "y" || s == "ya") return true;
        if (s == "t" || s == "tidak" || s == "n") return false;
        cout << "  ! Jawab y (ya) atau t (tidak).\n";
    }
}

// Membaca kata sandi tanpa menampilkannya bila input adalah terminal.
string bacaPassword(const string& prompt) {
    cout << prompt << flush;
#ifdef _WIN32
    string s;
    for (;;) {
        int c = _getch();
        if (c == '\r' || c == '\n') break;
        if (c == 3) throw InputBerakhir();  // Ctrl+C
        if (c == '\b') { if (!s.empty()) s.pop_back(); continue; }
        s += static_cast<char>(c);
    }
    cout << '\n';
    return s;
#else
    termios lama{};
    const bool terminal = isatty(STDIN_FILENO) && tcgetattr(STDIN_FILENO, &lama) == 0;
    if (terminal) {
        termios baru = lama;
        baru.c_lflag &= ~static_cast<tcflag_t>(ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &baru);
    }
    string s;
    const bool ok = static_cast<bool>(getline(cin, s));
    if (terminal) {
        tcsetattr(STDIN_FILENO, TCSANOW, &lama);
        cout << '\n';
    }
    if (!ok) throw InputBerakhir();
    if (!s.empty() && s.back() == '\r') s.pop_back();
    return s;
#endif
}

void tunggu() {
    cout << "\nTekan Enter untuk kembali..." << flush;
    ambilBaris();
}

// "dd-mm-yyyy HH:MM" -> time_t (zona waktu komputer). Kosong = batal.
bool bacaWaktu(const string& prompt, time_t& hasil) {
    for (;;) {
        const string s = bacaBaris(prompt + " [dd-mm-yyyy HH:MM, kosong=batal]: ");
        if (s.empty()) return false;
        int d, mo, y, h, mi;
        char sisa;
        if (sscanf(s.c_str(), "%d-%d-%d %d:%d%c", &d, &mo, &y, &h, &mi, &sisa) == 5 &&
            y >= 1970 && mo >= 1 && mo <= 12 && d >= 1 && d <= 31 && h >= 0 && h <= 23 && mi >= 0 && mi <= 59) {
            tm t{};
            t.tm_mday = d; t.tm_mon = mo - 1; t.tm_year = y - 1900;
            t.tm_hour = h; t.tm_min = mi; t.tm_isdst = -1;
            const time_t kandidat = mktime(&t);
            // mktime menormalkan tanggal mustahil (31-02 -> 03-03); tolak bila berubah
            if (kandidat != static_cast<time_t>(-1) && t.tm_mday == d && t.tm_mon == mo - 1) {
                hasil = kandidat;
                return true;
            }
        }
        cout << "  ! Format salah atau tanggal tidak ada. Contoh: 25-12-2021 23:59\n";
    }
}

// "HH:MM" -> HHMM (07:30 -> 730)
int bacaJam(const string& prompt) {
    for (;;) {
        const string s = bacaBaris(prompt + " [HH:MM]: ");
        int h, m;
        char sisa;
        if (sscanf(s.c_str(), "%d:%d%c", &h, &m, &sisa) == 2 && h >= 0 && h <= 23 && m >= 0 && m <= 59) {
            return h * 100 + m;
        }
        cout << "  ! Format salah. Contoh: 07:30\n";
    }
}

int pilihNomor(const string& prompt, int jumlah) {
    if (jumlah <= 0) return -1;
    const int n = bacaInt(prompt + " (0 = batal): ", 0, jumlah);
    return n == 0 ? -1 : n - 1;
}

// helper domain
string periodeTerbaru(const SistemAkademik& sistem) {
    string terbaru;
    for (const auto& k : sistem.getDaftarKelas()) {
        if (k->getPeriode() > terbaru) terbaru = k->getPeriode();
    }
    return terbaru;
}

string normalisasi(const string& teks) {
    string s = pangkasSpasi(teks);
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return tolower(c); });
    return s;
}

bool periodeSah(const string& p) {
    if (p.size() != 15 || p[4] != '/' || p[9] != '-') return false;
    for (int i : {0, 1, 2, 3, 5, 6, 7, 8}) if (!isdigit(static_cast<unsigned char>(p[i]))) return false;
    if (atoi(p.substr(5, 4).c_str()) != atoi(p.substr(0, 4).c_str()) + 1) return false;
    const string jenis = p.substr(10);
    return jenis == "Gasal" || jenis == "Genap";
}

string bacaPeriode(const string& prompt, const string& bawaan) {
    for (;;) {
        const string s = bacaBaris(prompt + (bawaan.empty() ? string(": ") : " [Enter=" + bawaan + "]: "));
        if (s.empty() && !bawaan.empty()) return bawaan;
        if (periodeSah(s)) return s;
        cout << "  ! Format periode: TTTT/TTTT-Gasal atau TTTT/TTTT-Genap (contoh 2021/2027-Gasal)\n";
    }
}

void layarGantiPassword(User& pengguna) {
    judul("GANTI KATA SANDI");
    const string lama = bacaPassword("  Kata sandi lama   : ");
    const string baru = bacaPassword("  Kata sandi baru   : ");
    const string ulang = bacaPassword("  Ulangi sandi baru : ");
    if (baru != ulang) { cout << "  ! Kata sandi baru tidak sama.\n"; return; }
    if (pengguna.gantiPassword(lama, baru)) cout << "  Kata sandi berhasil diganti.\n";
    else cout << "  ! Gagal: sandi lama salah atau sandi baru kurang dari 6 karakter.\n";
}

void cetakDaftarKelas(const vector<Kelas*>& daftar, bool tampilDosen) {
    cout << left << "  " << setw(4) << "No" << setw(15) << "Kelas" << setw(26) << "Mata Kuliah"
         << setw(5) << "SKS" << setw(20) << "Jadwal" << setw(9) << "Ruang" << setw(8) << "Peserta";
    if (tampilDosen) cout << "Dosen";
    cout << '\n';
    garis('-', tampilDosen ? 104 : 90);
    int no = 1;
    for (const Kelas* k : daftar) {
        const MataKuliah* mk = k->getMataKuliah();
        cout << "  " << left << setw(4) << no++ << setw(15) << pangkas(k->getId(), 14)
             << setw(26) << pangkas(mk ? mk->getNama() : "-", 25)
             << setw(5) << (mk ? mk->getSKS() : 0)
             << setw(20) << (k->punyaJadwal() ? k->getJadwal().toString() : "-")
             << setw(9) << (k->getRuangan() ? k->getRuangan()->getKode() : "-")
             << setw(8) << (to_string(k->jumlahPeserta()) + "/" + to_string(k->getKuota()));
        if (tampilDosen) cout << (k->getDosen() ? k->getDosen()->getNama() : "-");
        cout << '\n';
    }
}
}
