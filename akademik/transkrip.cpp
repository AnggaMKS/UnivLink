// RUMUS
//   IPS(periode) = SUM(bobotHuruf x SKS) / SUM(SKS)   atas Nilai FINAL di periode itu
//   IPK          = rumus yang sama atas "nilai terbaik per mata kuliah"
//   Jika SUM(SKS) = 0 hasilnya 0.0 (tidak membagi dengan nol).
//
// MENGULANG MATA KULIAH: bila satu mata kuliah punya beberapa Nilai final
// (mengulang), yang dipakai untuk IPK, prasyarat, dan SKS lulus adalah nilai
// dengan bobot TERTINGGI. Untuk IPS, semua nilai pada periode itu dihitung.
//
// PERIODE dibandingkan sebagai string. Format wajib "TTTT/TTTT-Gasal" atau
// "TTTT/TTTT-Genap": urutan abjad sama dengan urutan waktu
// ("2025/2021-Gasal" < "2025/2021-Genap" < "2021/2027-Gasal").

#include "univlink.h"

namespace {
// Nilai final yang punya kelas dan mata kuliah lengkap (data layak hitung).
bool layakHitung(const Nilai& n) {
    return n.isFinal() && n.getKelas() && n.getKelas()->getMataKuliah();
}
}

// Nilai terbaik (bobot tertinggi) untuk tiap kode mata kuliah, hanya yang final.
map<string, const Nilai*> Transkrip::nilaiTerbaikPerMK() const {
    map<string, const Nilai*> terbaik;
    for (const auto& n : listNilai) {
        if (!n || !layakHitung(*n)) continue;
        const string& kode = n->getKelas()->getMataKuliah()->getKode();
        auto it = terbaik.find(kode);
        if (it == terbaik.end() || n->getBobotHuruf() > it->second->getBobotHuruf()) {
            terbaik[kode] = n.get();
        }
    }
    return terbaik;
}

// Menyimpan Nilai baru. Satu kelas hanya boleh punya satu Nilai: duplikat dan pointer null diabaikan (objek yang diserahkan otomatis dihapus).
void Transkrip::tambahNilai(unique_ptr<Nilai> n) {
    if (!n) return;
    if (n->getKelas() && cariNilai(n->getKelas())) return;
    listNilai.push_back(std::move(n));
}

Nilai* Transkrip::cariNilai(const Kelas* kelas) const {
    for (const auto& n : listNilai) {
        if (n && n->getKelas() == kelas) return n.get();
    }
    return nullptr;
}

// Dipakai Dosen::inputNilai: mengembalikan Nilai milik kelas ini atau membuatnya.
Nilai* Transkrip::ambilAtauBuatNilai(Kelas* kelas) {
    if (!kelas) return nullptr;
    if (Nilai* ada = cariNilai(kelas)) return ada;
    listNilai.push_back(make_unique<Nilai>(kelas));
    return listNilai.back().get();
}

double Transkrip::hitungIPS(const string& periode) const {
    double totalMutu = 0.0;
    int totalSKS = 0;
    for (const auto& n : listNilai) {
        if (!n || !layakHitung(*n)) continue;
        if (n->getKelas()->getPeriode() != periode) continue;
        const int sks = n->getKelas()->getMataKuliah()->getSKS();
        totalMutu += n->getBobotHuruf() * sks;
        totalSKS += sks;
    }
    return totalSKS > 0 ? totalMutu / totalSKS : 0.0;
}

double Transkrip::hitungIPK() const {
    double totalMutu = 0.0;
    int totalSKS = 0;
    for (const auto& [kode, n] : nilaiTerbaikPerMK()) {
        const int sks = n->getKelas()->getMataKuliah()->getSKS();
        totalMutu += n->getBobotHuruf() * sks;
        totalSKS += sks;
    }
    return totalSKS > 0 ? totalMutu / totalSKS : 0.0;
}

// IPS pada periode TERBARU yang sudah punya nilai final. Mahasiswa baru
// (belum punya nilai final) mendapat 0.0.
double Transkrip::hitungIPSTerakhir() const {
    string terbaru;
    for (const auto& n : listNilai) {
        if (!n || !layakHitung(*n)) continue;
        const string& p = n->getKelas()->getPeriode();
        if (p > terbaru) terbaru = p;
    }
    return terbaru.empty() ? 0.0 : hitungIPS(terbaru);
}

// Lulus bila nilai TERBAIK mata kuliah itu lulus (lihat KonversiNilai::isLulus).
bool Transkrip::sudahLulus(const string& kodeMK) const {
    const auto terbaik = nilaiTerbaikPerMK();
    auto it = terbaik.find(kodeMK);
    return it != terbaik.end() && it->second->isLulus();
}

int Transkrip::totalSKSLulus() const {
    int total = 0;
    for (const auto& [kode, n] : nilaiTerbaikPerMK()) {
        if (n->isLulus()) total += n->getKelas()->getMataKuliah()->getSKS();
    }
    return total;
}

const vector<unique_ptr<Nilai>>& Transkrip::getListNilai() const { return listNilai; }
