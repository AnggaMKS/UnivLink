#include "univlink.h"
#include <algorithm>

// ===== Statistik Kelas =====
// method: hitungPersentaseKehadiran, hitungNilaiTugas, hitungDistribusiNilai


//hitungPersentaseKehadiran (%)
double Kelas::hitungPersentaseKehadiran(const Mahasiswa& mhs) const {
    if (totalPertemuan <= 0) return 0.0;
    if (!memilikiPeserta(mhs)) return 0.0;

    int jumlahHadir = 0;
    const string& niu = mhs.getNomorInduk();

    for (const auto& p : daftarPertemuan) {
        if (p && p->isHadir(niu)) {
            ++jumlahHadir;
        }
    }

    return (static_cast<double>(jumlahHadir) / totalPertemuan) * 100.0;
}

// hitungNilaiTugas: nilai rata-rata berbobot dari semua tugas yang sudah dikumpulkan dan dinilai 
double Kelas::hitungNilaiTugas(const Mahasiswa& mhs) const {
    double totalNilai = 0.0;
    double totalBobot = 0.0;

    for (const auto& t : daftarTugas) {
        if (!t) continue;

        Pengumpulan* p = t->getPengumpulan(mhs);
        if (!p || !p->isSudahDinilai()) continue;

        double bobot = t->getBobot();
        if (bobot <= 0.0) continue;

        totalNilai += p->getSkor() * bobot;
        totalBobot += bobot;
    }

    if (totalBobot <= 0.0) return 0.0;
    return totalNilai / totalBobot;
}

// hitungDistribusiNilai: Menghasilkan sebaran nilai huruf (A, A-, B+, …, E)
// dari seluruh mahasiswa yang sudah difinalisasi 
map<NilaiHuruf, int> Kelas::hitungDistribusiNilai() const {
    map<NilaiHuruf, int> distribusi;

    // Inisialisasi semua huruf agar selalu muncul (meski 0)
    distribusi[NilaiHuruf::A]      = 0;
    distribusi[NilaiHuruf::A_MIN]  = 0;
    distribusi[NilaiHuruf::A_B]    = 0;
    distribusi[NilaiHuruf::B_PLUS] = 0;
    distribusi[NilaiHuruf::B]      = 0;
    distribusi[NilaiHuruf::B_MIN]  = 0;
    distribusi[NilaiHuruf::B_C]    = 0;
    distribusi[NilaiHuruf::C_PLUS] = 0;
    distribusi[NilaiHuruf::C]      = 0;
    distribusi[NilaiHuruf::C_MIN]  = 0;
    distribusi[NilaiHuruf::D]      = 0;
    distribusi[NilaiHuruf::E]      = 0;

    for (Mahasiswa* mhs : getPeserta()) {
        if (!mhs) continue;

        // Nilai dimiliki oleh Transkrip mahasiswa
        const Nilai* n = mhs->getTranskrip().cariNilai(this);
        if (!n || !n->isFinal()) continue;

        distribusi[n->getNilaiHuruf()]++;
    }

    return distribusi;
}
