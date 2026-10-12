#include "univlink.h"
#include <algorithm>
#include <cmath>
#include <functional>


//  ===== MataKuliah =====

// Constructor identitas matkul
MataKuliah::MataKuliah(const string& kode, const string& nama, int sks, int semester)
    : kodeMK(kode), namaMK(nama), sks(sks), semester(semester) {}

// Getter untuk mengembalikan atribut
const string& MataKuliah::getKode() const { return kodeMK; }
const string& MataKuliah::getNama() const { return namaMK; }
int MataKuliah::getSKS() const { return sks; }
int MataKuliah::getSemester() const { return semester; }
vector<MataKuliah*> MataKuliah::getPrasyarat() const { return prasyarat; }
map<string, double> MataKuliah::getBobotKomponen() const { return bobotKomponen; }

// hitungRataRataBerbobot: rata-rata berbobot skor komponen
double MataKuliah::hitungRataRataBerbobot(const map<string, double>& komponen) const {
    double total = 0.0;
    double bobotTotal = 0.0;

    for (const auto& [nama, bobot] : bobotKomponen) {
        bobotTotal += bobot;
        auto it = komponen.find(nama);
        if (it != komponen.end()) {
            total += it->second * bobot;
        }
    }

    if (bobotTotal <= 0.0) return 0.0;
    return total / bobotTotal;
}

// setBobotKomponen: mengganti komposisi bobot penilaian
bool MataKuliah::setBobotKomponen(const map<string, double>& bobotBaru) {
    double total = 0.0;
    for (const auto& [nama, bobot] : bobotBaru) {
        if (nama.empty() || bobot < 0.0) return false;
        total += bobot;
    }
    if (fabs(total - 100.0) > 0.01) return false;

    bobotKomponen = bobotBaru;
    return true;
}

// ===== Alur prasyarat =====

bool MataKuliah::membutuhkan(const MataKuliah* target) const {
    for (const MataKuliah* p : prasyarat) {
        if (p == target || p->membutuhkan(target)) return true;
    }
    return false;
}

// tambahPrasyarat: mendaftarkan `mk` sebagai prasyarat mata kuliah
// Ditolak (false) jika:
//   1. mk null atau mk == this (prasyarat diri sendiri)
//   2. mk sudah ada di daftar (duplikat)
//   3. mk.membutuhkan(this)
bool MataKuliah::tambahPrasyarat(MataKuliah* mk) {
    if (!mk || mk == this) return false;
    if (find(prasyarat.begin(), prasyarat.end(), mk) != prasyarat.end()) return false;
    if (mk->membutuhkan(this)) return false;

    prasyarat.push_back(mk);
    return true;
}


// ===== MataKuliahTeori =====


// bobot default teori = tugas 20, UTS 30, UAS 40, kehadiran 10 (total 100).
MataKuliahTeori::MataKuliahTeori(const string& kode, const string& nama, int sks, int semester)
    : MataKuliah(kode, nama, sks, semester) {
    bobotKomponen = {{"tugas", 20.0}, {"uts", 30.0}, {"uas", 40.0}, {"kehadiran", 10.0}};
}

// Mata kuliah teori diajarkan di ruang kelas biasa. 
// Admin::assignRuangan untuk mencocokkan jenis ruangan.
JenisRuangan MataKuliahTeori::getJenisRuangan() const { return JenisRuangan::KELAS; }

// Nilai akhir teori = rata-rata berbobot biasa tanpa aturan tambahan.
double MataKuliahTeori::hitungNilaiAkhir(const map<string, double>& komponen) const {
    return hitungRataRataBerbobot(komponen);
}


// ===== MataKuliahPraktikum =====


// bobot default praktikum = tugas 30, praktik 40, laporan 20, kehadiran 10.
MataKuliahPraktikum::MataKuliahPraktikum(const string& kode, const string& nama, int sks, int semester)
    : MataKuliah(kode, nama, sks, semester) {
    bobotKomponen = {{"tugas", 30.0}, {"praktik", 40.0}, {"laporan", 20.0}, {"kehadiran", 10.0}};
}

// Praktikum wajib di laboratorium.
JenisRuangan MataKuliahPraktikum::getJenisRuangan() const { return JenisRuangan::LAB; }

// Nilai akhir praktikum = rata-rata berbobot + aturan kehadiran.
// Jika kehadiran < 75%, skor dibatasi maksimum 39 (jatuh ke E / tidak lulus) seberapa bagus nilai komponen lainnya.
double MataKuliahPraktikum::hitungNilaiAkhir(const map<string, double>& komponen) const {
    const double BATAS_KEHADIRAN = 75.0;
    const double SKOR_MAKS_JIKA_KURANG = 39.0;

    double nilai = hitungRataRataBerbobot(komponen);

    auto it = komponen.find("kehadiran");
    if (it != komponen.end() && it->second < BATAS_KEHADIRAN) {
        nilai = min(nilai, SKOR_MAKS_JIKA_KURANG);
    }
    return nilai;
}


// ===== Kurikulum =====

Kurikulum::Kurikulum(const string& prodi, int tahun) : prodi(prodi), tahun(tahun) {}

const string& Kurikulum::getProdi() const { return prodi; }
int Kurikulum::getTahun() const { return tahun; }

// tambahMataKuliah: memasukkan mk ke kurikulum, dikelompokkan di petaSemester[semester mk]
// Ditolak jika mk null, semester <= 0, atau kodenya sudah ada di kurikulum ini
bool Kurikulum::tambahMataKuliah(MataKuliah* mk) {
    if (!mk || mk->getSemester() <= 0) return false;
    if (cariMataKuliah(mk->getKode())) return false;

    petaSemester[mk->getSemester()].push_back(mk);
    return true;
}

// cariMataKuliah: menelusuri semua semester, mengembalikan mk dengan kode sama atau nullptr
MataKuliah* Kurikulum::cariMataKuliah(const string& kode) const {
    for (const auto& [smt, daftar] : petaSemester) {
        for (MataKuliah* mk : daftar) {
            if (mk->getKode() == kode) return mk;
        }
    }
    return nullptr;
}

// cariMataKuliah(semester): daftar mata kuliah di satu semester (kosong jika tidak ada)
vector<MataKuliah*> Kurikulum::cariMataKuliah(int semester) const {
    auto it = petaSemester.find(semester);
    if (it == petaSemester.end()) return {};
    return it->second;
}

// semuaMataKuliah: gabungan seluruh mata kuliah, urut semester 1, 2, 3, ... (map terurut by key)
vector<MataKuliah*> Kurikulum::semuaMataKuliah() const {
    vector<MataKuliah*> hasil;
    for (const auto& [smt, daftar] : petaSemester) {
        hasil.insert(hasil.end(), daftar.begin(), daftar.end());
    }
    return hasil;
}

// validasiGrafPrasyarat: memeriksa seluruh "peta prasyarat" kurikulum valid.
// Dianggap graf berarah (mk -> prasyaratnya), lalu dicek dengan DFS 3 warna:
// 0 = belum dikunjungi, 1 = sedang ditelusuri (ada di jalur DFS saat ini), 2 = selesai.
// Gagal (false) jika:
// - sebuah prasyarat tidak terdaftar di kurikulum ini (mis. prasyarat dari prodi lain), atau DFS bertemu simpul berwarna 1 lagi = ada siklus.

bool Kurikulum::validasiGrafPrasyarat() const {
    map<const MataKuliah*, int> warna;

    function<bool(const MataKuliah*)> dfs = [&](const MataKuliah* mk) -> bool {
        warna[mk] = 1;
        for (const MataKuliah* p : mk->getPrasyarat()) {
            if (cariMataKuliah(p->getKode()) != p) return false;  // prasyarat di luar kurikulum
            if (warna[p] == 1) return false;                      // siklus
            if (warna[p] == 0 && !dfs(p)) return false;
        }
        warna[mk] = 2;
        return true;
    };

    for (const MataKuliah* mk : semuaMataKuliah()) {
        if (warna[mk] == 0 && !dfs(mk)) return false;
    }
    return true;
}
