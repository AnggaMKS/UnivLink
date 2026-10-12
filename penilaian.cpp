#include "univlink.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

// ==============================
// KonversiNilai
// ==============================

NilaiHuruf KonversiNilai::skorKeHuruf(double skor) {
    if (skor >= 90.0) return NilaiHuruf::A;
    if (skor >= 85.0) return NilaiHuruf::A_MIN;
    if (skor >= 80.0) return NilaiHuruf::A_B;
    if (skor >= 75.0) return NilaiHuruf::B_PLUS;
    if (skor >= 70.0) return NilaiHuruf::B;
    if (skor >= 65.0) return NilaiHuruf::B_MIN;
    if (skor >= 60.0) return NilaiHuruf::B_C;
    if (skor >= 55.0) return NilaiHuruf::C_PLUS;
    if (skor >= 50.0) return NilaiHuruf::C;
    if (skor >= 45.0) return NilaiHuruf::C_MIN;
    if (skor >= 40.0) return NilaiHuruf::D;   
    return NilaiHuruf::E;
}

double KonversiNilai::hurufKeBobot(NilaiHuruf h) {
    switch (h) {
        case NilaiHuruf::A:      return 4.00;
        case NilaiHuruf::A_MIN:  return 3.75;
        case NilaiHuruf::A_B:    return 3.50;
        case NilaiHuruf::B_PLUS: return 3.25;
        case NilaiHuruf::B:      return 3.00;
        case NilaiHuruf::B_MIN:  return 2.75;
        case NilaiHuruf::B_C:    return 2.50;
        case NilaiHuruf::C_PLUS: return 2.25;
        case NilaiHuruf::C:      return 2.00;
        case NilaiHuruf::C_MIN:  return 1.75;
        case NilaiHuruf::D:      return 1.00;
        case NilaiHuruf::E:      return 0.00;
        default:                 return 0.00;
    }
}

string KonversiNilai::hurufKeString(NilaiHuruf h) {
    switch (h) {
        case NilaiHuruf::A:      return "A";
        case NilaiHuruf::A_MIN:  return "A-";
        case NilaiHuruf::A_B:    return "A/B";
        case NilaiHuruf::B_PLUS: return "B+";
        case NilaiHuruf::B:      return "B";
        case NilaiHuruf::B_MIN:  return "B-";
        case NilaiHuruf::B_C:    return "B/C";
        case NilaiHuruf::C_PLUS: return "C+";
        case NilaiHuruf::C:      return "C";
        case NilaiHuruf::C_MIN:  return "C-";
        case NilaiHuruf::D:      return "D";
        case NilaiHuruf::E:      return "E";
        default:                 return "-";
    }
}

bool KonversiNilai::isLulus(NilaiHuruf h) {
    // Dianggap lulus jika bukan E 
    return h != NilaiHuruf::E;
}

// ==============================
// Nilai
// ==============================

Nilai::Nilai(Kelas* kelas)
    : kelas(kelas),
      skorAkhir(0.0),
      nilaiHuruf(NilaiHuruf::E),
      bobotHuruf(0.0),
      sudahFinal(false) {
    if (!kelas) {
        throw invalid_argument("Nilai harus terikat ke Kelas yang valid");
    }
}

bool Nilai::updateKomponen(const string& nama, double skor) {
    if (sudahFinal) return false;               // sudah dikunci
    if (skor < 0.0 || skor > 100.0) return false;

    komponen[nama] = skor;
    return true;
}

double Nilai::hitungNilaiAkhir() {
    if (!kelas || !kelas->getMataKuliah()) {
        skorAkhir = 0.0;
        return skorAkhir;
    }

    skorAkhir = kelas->getMataKuliah()->hitungNilaiAkhir(komponen);
    return skorAkhir;
}

bool Nilai::finalisasi() {
    if (sudahFinal) return false;

    hitungNilaiAkhir();
    nilaiHuruf = KonversiNilai::skorKeHuruf(skorAkhir);
    bobotHuruf = KonversiNilai::hurufKeBobot(nilaiHuruf);
    sudahFinal = true;
    return true;
}

bool Nilai::isFinal() const {
    return sudahFinal;
}

bool Nilai::isLulus() const {
    return KonversiNilai::isLulus(nilaiHuruf);
}

NilaiHuruf Nilai::getNilaiHuruf() const {
    return nilaiHuruf;
}

double Nilai::getBobotHuruf() const {
    return bobotHuruf;
}

double Nilai::getSkorAkhir() const {
    return skorAkhir;
}

Kelas* Nilai::getKelas() const {
    return kelas;
}

const map<string, double>& Nilai::getKomponen() const {
    return komponen;
}

// ==============================
// Transkrip
// ==============================

void Transkrip::tambahNilai(unique_ptr<Nilai> n) {
    if (!n) return;
    listNilai.push_back(std::move(n));
}

Nilai* Transkrip::cariNilai(const Kelas* kelas) const {
    if (!kelas) return nullptr;
    for (const auto& n : listNilai) {
        if (n && n->getKelas() == kelas) {
            return n.get();
        }
    }
    return nullptr;
}

Nilai* Transkrip::ambilAtauBuatNilai(Kelas* kelas) {
    if (!kelas) return nullptr;

    Nilai* existing = cariNilai(kelas);
    if (existing) return existing;

    auto baru = make_unique<Nilai>(kelas);
    Nilai* ptr = baru.get();
    listNilai.push_back(std::move(baru));
    return ptr;
}

map<string, const Nilai*> Transkrip::nilaiTerbaikPerMK() const {
    map<string, const Nilai*> terbaik;

    for (const auto& n : listNilai) {
        if (!n || !n->isFinal() || !n->getKelas() || !n->getKelas()->getMataKuliah()) {
            continue;
        }

        const string& kode = n->getKelas()->getMataKuliah()->getKode();
        auto it = terbaik.find(kode);

        if (it == terbaik.end()) {
            terbaik[kode] = n.get();
        } else {
            // Ambil yang bobotnya lebih tinggi (atau skor lebih tinggi jika sama)
            if (n->getBobotHuruf() > it->second->getBobotHuruf() ||
                (n->getBobotHuruf() == it->second->getBobotHuruf() &&
                 n->getSkorAkhir() > it->second->getSkorAkhir())) {
                terbaik[kode] = n.get();
            }
        }
    }
    return terbaik;
}

double Transkrip::hitungIPS(const string& periode) const {
    double totalBobotSKS = 0.0;
    int totalSKS = 0;

    for (const auto& n : listNilai) {
        if (!n || !n->isFinal()) continue;
        if (!n->getKelas() || n->getKelas()->getPeriode() != periode) continue;
        if (!n->getKelas()->getMataKuliah()) continue;

        int sks = n->getKelas()->getMataKuliah()->getSKS();
        totalBobotSKS += n->getBobotHuruf() * sks;
        totalSKS += sks;
    }

    if (totalSKS == 0) return 0.0;
    return totalBobotSKS / totalSKS;
}

double Transkrip::hitungIPK() const {
    auto terbaik = nilaiTerbaikPerMK();

    double totalBobotSKS = 0.0;
    int totalSKS = 0;

    for (const auto& pair : terbaik) {
        const Nilai* n = pair.second;
        if (!n || !n->getKelas() || !n->getKelas()->getMataKuliah()) continue;

        int sks = n->getKelas()->getMataKuliah()->getSKS();
        totalBobotSKS += n->getBobotHuruf() * sks;
        totalSKS += sks;
    }

    if (totalSKS == 0) return 0.0;
    return totalBobotSKS / totalSKS;
}

double Transkrip::hitungIPSTerakhir() const {
    // Cari periode paling baru yang memiliki nilai final
    string periodeTerakhir;
    for (const auto& n : listNilai) {
        if (!n || !n->isFinal() || !n->getKelas()) continue;
        const string& p = n->getKelas()->getPeriode();
        if (p > periodeTerakhir) {   // string comparison cukup untuk format "2025/2026-1"
            periodeTerakhir = p;
        }
    }

    if (periodeTerakhir.empty()) return 0.0;
    return hitungIPS(periodeTerakhir);
}

bool Transkrip::sudahLulus(const string& kodeMK) const {
    auto terbaik = nilaiTerbaikPerMK();
    auto it = terbaik.find(kodeMK);
    if (it == terbaik.end()) return false;
    return it->second->isLulus();
}

int Transkrip::totalSKSLulus() const {
    auto terbaik = nilaiTerbaikPerMK();
    int total = 0;

    for (const auto& pair : terbaik) {
        const Nilai* n = pair.second;
        if (n && n->isLulus() && n->getKelas() && n->getKelas()->getMataKuliah()) {
            total += n->getKelas()->getMataKuliah()->getSKS();
        }
    }
    return total;
}

const vector<unique_ptr<Nilai>>& Transkrip::getListNilai() const {
    return listNilai;
}
