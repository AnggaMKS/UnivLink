#include "cli.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
using namespace std;
using namespace cli;

// ===== Mahasiswa::tampilkanDashboard =====
// Menampilkan ringkasan akademik mahasiswa: Identitas & IPK / IPS / batas SKS,
// Jadwal kuliah semester berjalan (ringkas), Pengumuman aktif terbaru
void Mahasiswa::tampilkanDashboard() const {
    cetakHeader();
    judul("DASHBOARD MAHASISWA");

    cout << fixed << setprecision(2);
    cout << "  Angkatan      : " << angkatan << '\n';
    cout << "  Semester      : " << semesterBerjalan << '\n';
    cout << '\n';
    cout << "  IPK           : " << getIPK() << '\n';
    cout << "  IPS Terakhir  : " << getIPSTerakhir() << '\n';
    cout << "  Batas SKS     : " << hitungMaxSKS() << " SKS\n";
    cout << "  Total SKS Lulus: " << transkrip.totalSKSLulus() << " SKS\n";

    // Jadwal semester ini 
    cout << '\n';
    garis();
    cout << "  JADWAL SEMESTER INI\n";
    garis();

    vector<Kelas*> jadwal = getJadwalSemesterIni();
    if (jadwal.empty()) {
        cout << "  (Belum ada mata kuliah yang diambil)\n";
    } else {
        cout << left << setw(12) << "  Kode"
             << setw(28) << "Mata Kuliah"
             << setw(8)  << "SKS"
             << "Jadwal\n";
        garis('-');
        for (Kelas* k : jadwal) {
            if (!k || !k->getMataKuliah()) continue;
            const MataKuliah* mk = k->getMataKuliah();
            cout << "  " << left << setw(10) << mk->getKode()
                 << setw(28) << mk->getNama().substr(0, 26)
                 << setw(8)  << mk->getSKS()
                 << (k->punyaJadwal() ? k->getJadwal().toString() : "-")
                 << '\n';
        }
    }

    // Pengumuman aktif
    cout << '\n';
    garis();
    cout << "  PENGUMUMAN AKTIF\n";
    garis();

    vector<Pengumuman*> peng = getPengumumanAktif();
    if (peng.empty()) {
        cout << "  (Tidak ada pengumuman aktif)\n";
    } else {
        int maxTampil = min(5, static_cast<int>(peng.size()));
        for (int i = 0; i < maxTampil; ++i) {
            Pengumuman* p = peng[i];
            if (!p) continue;
            cout << "  [" << formatWaktu(p->getTanggal()) << "] "
                 << p->getJudul() << '\n';
            // potong isi jika terlalu panjang
            string isi = p->getIsi();
            if (isi.size() > 80) isi = isi.substr(0, 77) + "...";
            cout << "    " << isi << '\n';
            if (i + 1 < maxTampil) cout << '\n';
        }
        if (static_cast<int>(peng.size()) > maxTampil) {
            cout << "  ... dan " << (peng.size() - maxTampil) << " pengumuman lainnya\n";
        }
    }

    cout << '\n';
    garis('=');
}

// ===== Mahasiswa::tampilkanMenu =====

// Daftar pilihan menu CLI untuk mahasiswa.
void Mahasiswa::tampilkanMenu() const {
    cout << '\n';
    garis('=');
    cout << "  MENU MAHASISWA\n";
    garis('=');
    cout << "  1. Lihat Dashboard\n";
    cout << "  2. Registrasi Mata Kuliah (KRS)\n";
    cout << "  3. Lihat / Batalkan KRS\n";
    cout << "  4. Lihat Transkrip Nilai\n";
    cout << "  5. Detail Kelas & Portal Tugas\n";
    cout << "  6. Lihat Pengumuman\n";
    cout << "  7. Ganti Kata Sandi\n";
    cout << "  0. Logout\n";
    garis('-');
    cout << "  Pilihan Anda: ";
}


// ===== Method pendukung dashboard =====

int Mahasiswa::hitungMaxSKS() const {
    constexpr int SKS_SEMESTER_PERTAMA = 20;

    bool adaNilaiFinal = false;
    for (const auto& n : transkrip.getListNilai()) {
        if (n && n->isFinal()) { adaNilaiFinal = true; break; }
    }
    if (!adaNilaiFinal) return SKS_SEMESTER_PERTAMA;

    const double ips = std::round(getIPSTerakhir() * 100.0) / 100.0;
    if (ips >= 3.00) return 24;
    if (ips >= 2.50) return 21;
    if (ips >= 2.00) return 18;
    return 15;
}

bool Mahasiswa::cekPrasyarat(const MataKuliah& mk) const {
    for (MataKuliah* pre : mk.getPrasyarat()) {
        if (!pre) continue;
        if (!transkrip.sudahLulus(pre->getKode())) {
            return false;
        }
    }
    return true;
}

double Mahasiswa::getIPK() const {
    return transkrip.hitungIPK();
}

double Mahasiswa::getIPSTerakhir() const {
    return transkrip.hitungIPSTerakhir();
}

void Mahasiswa::lihatTranskrip() const {
    judul("TRANSKRIP NILAI");

    vector<const Nilai*> final;
    for (const auto& n : transkrip.getListNilai()) {
        if (n && n->isFinal() && n->getKelas() && n->getKelas()->getMataKuliah()) final.push_back(n.get());
    }
    if (final.empty()) {
        cout << "  Belum ada nilai yang tercatat.\n";
        return;
    }
    sort(final.begin(), final.end(), [](const Nilai* a, const Nilai* b) {
        if (a->getKelas()->getPeriode() != b->getKelas()->getPeriode())
            return a->getKelas()->getPeriode() < b->getKelas()->getPeriode();
        return a->getKelas()->getMataKuliah()->getKode() < b->getKelas()->getMataKuliah()->getKode();
    });

    cout << left
         << setw(12) << "  Kode"
         << setw(28) << "Mata Kuliah"
         << setw(6)  << "SKS"
         << setw(8)  << "Huruf"
         << setw(8)  << "Bobot"
         << "Periode\n";
    garis('-');

    cout << fixed << setprecision(2);
    for (const Nilai* n : final) {
        const MataKuliah* mk = n->getKelas()->getMataKuliah();
        cout << "  " << left
             << setw(10) << mk->getKode()
             << setw(28) << mk->getNama().substr(0, 26)
             << setw(6)  << mk->getSKS()
             << setw(8)  << KonversiNilai::hurufKeString(n->getNilaiHuruf())
             << setw(8)  << n->getBobotHuruf()
             << n->getKelas()->getPeriode() << '\n';
        cout << "      sebaran kelas: " << ringkasDistribusi(*n->getKelas()) << '\n';
    }

    cout << '\n';
    cout << "  IPS Terakhir  : " << getIPSTerakhir() << '\n';
    cout << "  IPK           : " << getIPK() << '\n';
    cout << "  Total SKS Lulus: " << transkrip.totalSKSLulus() << '\n';
    garis('=');
}

void Mahasiswa::lihatDetailKelas(const Kelas& kelas) const {
    if (!kelas.getMataKuliah()) {
        cout << "  Data kelas tidak lengkap.\n";
        return;
    }

    const MataKuliah* mk = kelas.getMataKuliah();
    judul("DETAIL KELAS: " + mk->getNama());

    cout << "  Kode MK       : " << mk->getKode() << '\n';
    cout << "  SKS           : " << mk->getSKS() << '\n';
    cout << "  Dosen         : " << (kelas.getDosen() ? kelas.getDosen()->getNama() : "-") << '\n';
    cout << "  Jadwal        : " << (kelas.punyaJadwal() ? kelas.getJadwal().toString() : "-") << '\n';
    cout << "  Ruangan       : " << (kelas.getRuangan() ? kelas.getRuangan()->getKode() : "-") << '\n';
    cout << '\n';

    // Kehadiran
    double persen = kelas.hitungPersentaseKehadiran(*this);
    cout << fixed << setprecision(1);
    cout << "  Kehadiran     : " << persen << "%\n";

    // Nilai tugas
    double nilaiTugas = kelas.hitungNilaiTugas(*this);
    cout << "  Nilai Tugas   : " << setprecision(2) << nilaiTugas << '\n';
    cout << '\n';
    garis();
    cout << "  RINCIAN NILAI\n";
    garis();
    const Nilai* nilai = transkrip.cariNilai(&kelas);
    cout << fixed << setprecision(2);
    for (const auto& [nama, bobotPersen] : mk->getBobotKomponen()) {
        cout << "  " << left << setw(12) << nama << "(bobot " << setw(5) << bobotPersen << "%)  ";
        const double* skorTersimpan = nullptr;
        if (nilai) {
            auto it = nilai->getKomponen().find(nama);
            if (it != nilai->getKomponen().end()) skorTersimpan = &it->second;
        }
        if (skorTersimpan) {
            cout << *skorTersimpan << '\n';
        } else if (nama == "tugas") {
            cout << nilaiTugas << "  (sementara, dari tugas yang sudah dinilai)\n";
        } else if (nama == "kehadiran") {
            cout << persen << "  (sementara, dari presensi)\n";
        } else {
            cout << "-  (belum diinput dosen)\n";
        }
    }
    if (nilai && nilai->isFinal()) {
        cout << "  Nilai akhir   : " << nilai->getSkorAkhir()
             << "  ->  " << KonversiNilai::hurufKeString(nilai->getNilaiHuruf())
             << "  (bobot " << nilai->getBobotHuruf() << ")\n";
        cout << "  Sebaran kelas : " << ringkasDistribusi(kelas) << '\n';
    } else {
        cout << "  Nilai akhir   : belum difinalisasi dosen\n";
    }

    // Daftar tugas
    cout << '\n';
    garis();
    cout << "  DAFTAR TUGAS\n";
    garis();

    vector<Tugas*> tugas = kelas.getDaftarTugas();
    if (tugas.empty()) {
        cout << "  (Belum ada tugas)\n";
    } else {
        for (Tugas* t : tugas) {
            if (!t) continue;
            cout << "  [" << t->getId() << "] " << t->getJudul() << '\n';
            cout << "    Deadline : " << formatWaktu(t->getDeadline());
            if (t->isDeadlineLewat()) cout << "  (LEWAT)";
            cout << '\n';
            cout << "    Bobot    : " << t->getBobot() << '\n';

            Pengumpulan* p = t->getPengumpulan(*this);
            if (p) {
                cout << "    Status   : Sudah dikumpulkan ("
                     << formatWaktu(p->getWaktuKumpul()) << ")";
                if (p->isSudahDinilai()) {
                    cout << " | Skor: " << p->getSkor();
                } else {
                    cout << " | Belum dinilai";
                }
                cout << '\n';
            } else {
                cout << "    Status   : Belum dikumpulkan\n";
            }
            cout << '\n';
        }
    }
    garis('=');
}

vector<Kelas*> Mahasiswa::getJadwalSemesterIni() const {
    vector<Kelas*> hasil;
    const KRS* krsAktif = nullptr;
    for (const auto& k : daftarKRS) {
        if (!k) continue;
        if (!krsAktif || k->getPeriode() > krsAktif->getPeriode()) {
            krsAktif = k.get();
        }
    }
    if (!krsAktif) return hasil;

    for (Kelas* kelas : krsAktif->getDaftarKelas()) {
        if (kelas) hasil.push_back(kelas);
    }
    return hasil;
}

vector<Pengumuman*> Mahasiswa::getPengumumanAktif() const {
    vector<Pengumuman*> hasil;
    vector<Kelas*> jadwal = getJadwalSemesterIni();

    for (Kelas* k : jadwal) {
        if (!k) continue;
        for (Pengumuman* p : k->getPengumumanAktif()) {
            if (p) hasil.push_back(p);
        }
    }

    // Urutkan dari yang terbaru
    sort(hasil.begin(), hasil.end(),
         [](const Pengumuman* a, const Pengumuman* b) {
             return a->getTanggal() > b->getTanggal();
         });

    return hasil;
}

// Hanya peserta kelas pemilik tugas yang boleh mengumpulkan 
bool Mahasiswa::submitTugas(Tugas& tugas, const string& file) {
    if (file.empty()) return false;
    for (const auto& krs : daftarKRS) {
        if (!krs) continue;
        for (const Kelas* kelas : krs->getDaftarKelas()) {
            if (kelas && kelas->memilikiTugas(&tugas) && kelas->memilikiPeserta(*this)) {
                return tugas.submit(*this, file);
            }
        }
    }
    return false;
}


// ===== KRS helpers =====

KRS* Mahasiswa::cariKRS(const string& periode) const {
    for (const auto& k : daftarKRS) {
        if (k && k->getPeriode() == periode) return k.get();
    }
    return nullptr;
}

KRS* Mahasiswa::buatKRS(const string& periode) {
    if (cariKRS(periode)) return nullptr; // sudah ada
    auto krs = make_unique<KRS>(this, periode);
    KRS* ptr = krs.get();
    daftarKRS.push_back(std::move(krs));
    return ptr;
}

bool Mahasiswa::ambilKelas(Kelas& kelas) {
    // Cari atau buat KRS untuk periode kelas
    KRS* krs = cariKRS(kelas.getPeriode());
    if (!krs) {
        krs = buatKRS(kelas.getPeriode());
        if (!krs) return false;
    }
    return krs->tambahKelas(&kelas);
}

bool Mahasiswa::batalKelas(Kelas& kelas) {
    KRS* krs = cariKRS(kelas.getPeriode());
    if (!krs) return false;
    return krs->hapusKelas(&kelas);
}

// Getter 
int Mahasiswa::getAngkatan() const { return angkatan; }
int Mahasiswa::getSemesterBerjalan() const { return semesterBerjalan; }
Kurikulum* Mahasiswa::getKurikulum() const { return kurikulum; }
Transkrip& Mahasiswa::getTranskrip() { return transkrip; }
const Transkrip& Mahasiswa::getTranskrip() const { return transkrip; }
const vector<unique_ptr<KRS>>& Mahasiswa::getDaftarKRS() const { return daftarKRS; }

string Mahasiswa::getPeran() const { return "Mahasiswa"; }
