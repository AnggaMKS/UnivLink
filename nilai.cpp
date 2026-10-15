// DATA
//   komponen    map<string,double>  nama komponen -> skor 0..100
//                                   (nama sesuai MataKuliah::getBobotKomponen)
//   skorAkhir, nilaiHuruf, bobotHuruf : atribut TURUNAN. Hanya diubah lewat
//                                   hitungNilaiAkhir() / finalisasi().
//   sudahFinal  : setelah true, nilai terkunci (tidak bisa diubah lagi).
//
// ALUR HIDUP
//   dibuat kosong -> dosen mengisi komponen (updateKomponen) -> finalisasi() menghitung skor akhir, huruf, bobot lalu mengunci.

#include "univlink.h"
#include <cmath>
namespace {
bool skorSah(double s) { return s >= 0.0 && s <= 100.0; }
}  

Nilai::Nilai(Kelas* kelas)
    : kelas(kelas), skorAkhir(0.0), nilaiHuruf(NilaiHuruf::E), bobotHuruf(0.0), sudahFinal(false) {}

bool Nilai::updateKomponen(const string& nama, double skor) {
    if (sudahFinal) return false;
    if (nama.empty() || !skorSah(skor)) return false;

    if (kelas && kelas->getMataKuliah()) {
        const map<string, double> bobot = kelas->getMataKuliah()->getBobotKomponen();
        if (bobot.find(nama) == bobot.end()) return false;
    }
    komponen[nama] = skor;
    return true;
}

// Menghitung ulang skorAkhir, nilaiHuruf, bobotHuruf dari komponen saat ini.
double Nilai::hitungNilaiAkhir() {
    if (!kelas || !kelas->getMataKuliah()) return skorAkhir;

    double mentah = kelas->getMataKuliah()->hitungNilaiAkhir(komponen);
    if (mentah < 0.0) mentah = 0.0;
    if (mentah > 100.0) mentah = 100.0;

    skorAkhir = std::round(mentah * 100.0) / 100.0;
    nilaiHuruf = KonversiNilai::skorKeHuruf(skorAkhir);
    bobotHuruf = KonversiNilai::hurufKeBobot(nilaiHuruf);
    return skorAkhir;
}

// Mengunci nilai. false bila sudah final atau kelas/mata kuliah tidak diketahui.
bool Nilai::finalisasi() {
    if (sudahFinal) return false;
    if (!kelas || !kelas->getMataKuliah()) return false;
    hitungNilaiAkhir();
    sudahFinal = true;
    return true;
}

bool Nilai::isFinal() const { return sudahFinal; }
bool Nilai::isLulus() const { return sudahFinal && KonversiNilai::isLulus(nilaiHuruf); }
NilaiHuruf Nilai::getNilaiHuruf() const { return nilaiHuruf; }
double Nilai::getBobotHuruf() const { return bobotHuruf; }
double Nilai::getSkorAkhir() const { return skorAkhir; }
Kelas* Nilai::getKelas() const { return kelas; }
const map<string, double>& Nilai::getKomponen() const { return komponen; }
