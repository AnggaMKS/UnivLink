// cli_dosen_menu.cpp
//pengumuman, penugasan, penilaian, kehadiran.

#include "cli.h"
#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <sstream>
using namespace cli;
namespace {

bool bacaSkorOpsional(const string& prompt, double& skor) {
    for (;;) {
        string s = bacaBaris(prompt);
        if (s.empty()) return false;
        replace(s.begin(), s.end(), ',', '.');
        char* akhir = nullptr;
        const double v = strtod(s.c_str(), &akhir);
        if (*akhir == '\0' && v >= 0.0 && v <= 100.0) { skor = v; return true; }
        cout << "  ! Skor harus angka 0-100 (kosong = lewati).\n";
    }
}

// Memilih kelas yang diampu; terbaru dulu. nullptr bila batal / tidak ada.
Kelas* pilihKelas(const Dosen& dosen) {
    vector<Kelas*> daftar = dosen.getKelasDiampu();
    if (daftar.empty()) {
        cout << "\n  Anda belum mengampu kelas apa pun. Hubungi Admin.\n";
        tunggu();
        return nullptr;
    }
    sort(daftar.begin(), daftar.end(), [](const Kelas* a, const Kelas* b) {
        if (a->getPeriode() != b->getPeriode()) return a->getPeriode() > b->getPeriode();
        return a->getId() < b->getId();
    });
    judul("PILIH KELAS");
    cetakDaftarKelas(daftar, false);
    const int idx = pilihNomor("\n  Pilih kelas", static_cast<int>(daftar.size()));
    return idx < 0 ? nullptr : daftar[idx];
}

// mahasiswa urut NIU.
vector<Mahasiswa*> pesertaUrut(const Kelas& k) {
    vector<Mahasiswa*> p = k.getPeserta();
    sort(p.begin(), p.end(), [](const Mahasiswa* a, const Mahasiswa* b) { return a->getNomorInduk() < b->getNomorInduk(); });
    return p;
}

void laporGagal(const Dosen& d) {
    cout << "  ! Ditolak: " << (d.getPesanTerakhir().empty() ? "operasi gagal" : d.getPesanTerakhir()) << '\n';
}

// announcement
void layarPengumuman(Dosen& dosen, Kelas& kelas) {
    for (;;) {
        judul("PENGUMUMAN - " + kelas.getId());
        const vector<Pengumuman*> daftar = kelas.getDaftarPengumuman();
        if (daftar.empty()) cout << "  (belum ada pengumuman)\n";
        for (size_t i = 0; i < daftar.size(); ++i) {
            const Pengumuman* p = daftar[i];
            cout << "  " << (i + 1) << ". [" << (p->getStatus() == StatusPengumuman::TERBIT ? "TERBIT" : "DRAFT ")
                 << "] " << p->getJudul() << "  (" << formatWaktu(p->getTanggal()) << ")\n"
                 << "       " << p->getIsi() << '\n';
        }
        cout << "\n  1. Buat pengumuman baru\n  2. Edit isi pengumuman\n  3. Terbitkan pengumuman\n  0. Kembali\n";
        const int p = bacaInt("  Pilihan: ", 0, 3);
        if (p == 0) return;

        if (p == 1) {
            const string j = bacaBaris("  Judul (kosong = batal): ");
            if (j.empty()) continue;
            const string isi = bacaTidakKosong("  Isi: ");
            if (dosen.buatPengumuman(kelas, j, isi)) cout << "  Pengumuman dibuat sebagai DRAFT. Terbitkan agar terlihat mahasiswa.\n";
            else laporGagal(dosen);
        } else {
            const int idx = pilihNomor("  Nomor pengumuman", static_cast<int>(daftar.size()));
            if (idx < 0) continue;
            if (p == 2) {
                const string isi = bacaBaris("  Isi baru (kosong = batal): ");
                if (isi.empty()) continue;
                if (dosen.editPengumuman(*daftar[idx], isi)) cout << "  Pengumuman diperbarui.\n";
                else laporGagal(dosen);
            } else {
                if (dosen.publikasikanPengumuman(*daftar[idx])) cout << "  Pengumuman diterbitkan.\n";
                else laporGagal(dosen);
            }
        }
        tunggu();
    }
}

// penugasan
void tampilkanPengumpulan(const Tugas& t) {
    cout << "\n  Pengumpulan \"" << t.getJudul() << "\" (tenggat " << formatWaktu(t.getDeadline()) << ")\n";
    const vector<Pengumpulan*> daftar = t.getDaftarPengumpulan();
    if (daftar.empty()) { cout << "  (belum ada yang mengumpulkan)\n"; return; }
    cout << "  " << left << setw(4) << "No" << setw(14) << "NIU" << setw(24) << "Nama" << setw(18) << "Waktu"
         << setw(10) << "Skor" << "Berkas\n";
    garis('-', 90);
    for (size_t i = 0; i < daftar.size(); ++i) {
        const Pengumpulan* g = daftar[i];
        const Mahasiswa* m = g->getPengumpul();
        cout << "  " << left << setw(4) << (i + 1) << setw(14) << (m ? m->getNomorInduk() : "-")
             << setw(24) << pangkas(m ? m->getNama() : "-", 23) << setw(18) << formatWaktu(g->getWaktuKumpul());
        if (g->isSudahDinilai()) cout << setw(10) << fixed << setprecision(1) << g->getSkor();
        else cout << setw(10) << "-";
        cout << g->getFilePath() << '\n';
    }
}

void layarPenugasan(Dosen& dosen, Kelas& kelas) {
    for (;;) {
        judul("PENUGASAN - " + kelas.getId());
        const vector<Tugas*> tugas = kelas.getDaftarTugas();
        if (tugas.empty()) cout << "  (belum ada tugas)\n";
        for (size_t i = 0; i < tugas.size(); ++i) {
            cout << "  " << (i + 1) << ". " << tugas[i]->getJudul() << "  | bobot " << tugas[i]->getBobot()
                 << "  | tenggat " << formatWaktu(tugas[i]->getDeadline())
                 << (tugas[i]->isDeadlineLewat() ? " (lewat)" : "")
                 << "  | terkumpul " << tugas[i]->getDaftarPengumpulan().size() << "/" << kelas.jumlahPeserta() << '\n';
        }
        cout << "\n  1. Buat tugas baru\n  2. Lihat pengumpulan\n  3. Beri skor tugas\n  0. Kembali\n";
        const int p = bacaInt("  Pilihan: ", 0, 3);
        if (p == 0) return;

        if (p == 1) {
            const string j = bacaBaris("  Judul tugas (kosong = batal): ");
            if (j.empty()) continue;
            const string desk = bacaBaris("  Deskripsi: ");
            time_t deadline = 0;
            if (!bacaWaktu("  Tenggat", deadline)) continue;
            const double bobot = bacaDouble("  Bobot (>0 - 100): ", 0.01, 100.0);
            if (dosen.buatTugas(kelas, j, desk, deadline, bobot)) cout << "  Tugas dibuat.\n";
            else laporGagal(dosen);
        } else {
            const int idx = pilihNomor("  Nomor tugas", static_cast<int>(tugas.size()));
            if (idx < 0) continue;
            Tugas& t = *tugas[idx];
            tampilkanPengumpulan(t);
            if (p == 3) {
                const vector<Pengumpulan*> kumpul = t.getDaftarPengumpulan();
                const int g = pilihNomor("\n  Nomor pengumpulan yang dinilai", static_cast<int>(kumpul.size()));
                if (g < 0) continue;
                Mahasiswa* m = kumpul[g]->getPengumpul();
                if (!m) { cout << "  ! Data pengumpul tidak lengkap.\n"; tunggu(); continue; }
                const double skor = bacaDouble("  Skor (0-100): ", 0.0, 100.0);
                if (dosen.beriSkorTugas(t, *m, skor)) cout << "  Skor tersimpan.\n";
                else laporGagal(dosen);
            }
        }
        tunggu();
    }
}

// penilaian
// Skor satu peserta: komponen tersimpan, atau nilai sementara untuk tugas/kehadiran.
void rekapNilai(const Kelas& kelas) {
    const MataKuliah* mk = kelas.getMataKuliah();
    const map<string, double> bobot = mk->getBobotKomponen();
    judul("REKAP NILAI - " + kelas.getId() + " (" + mk->getNama() + ")");

    cout << "  " << left << setw(12) << "NIU" << setw(22) << "Nama";
    for (const auto& [nama, b] : bobot) cout << setw(16) << (nama + "(" + to_string(static_cast<int>(b)) + ")");
    cout << setw(8) << "Akhir" << "Huruf\n";
    garis('-', 12 + 22 + static_cast<int>(bobot.size()) * 16 + 14);

    for (const Mahasiswa* m : pesertaUrut(kelas)) {
        const Nilai* n = m->getTranskrip().cariNilai(&kelas);
        const map<string, double> tersimpan = n ? n->getKomponen() : map<string, double>{};
        cout << "  " << left << setw(12) << m->getNomorInduk() << setw(22) << pangkas(m->getNama(), 21);
        for (const auto& [nama, b] : bobot) {
            (void)b;
            auto it = tersimpan.find(nama);
            if (it != tersimpan.end()) {
                cout << setw(16) << fixed << setprecision(1) << it->second;
            } else if (n && n->isFinal()) {
                cout << setw(16) << "0.0";
            } else if (nama == "tugas") {
                ostringstream os;
                os << fixed << setprecision(1) << kelas.hitungNilaiTugas(*m) << "*";
                cout << setw(16) << os.str();
            } else if (nama == "kehadiran") {
                ostringstream os;
                os << fixed << setprecision(1) << kelas.hitungPersentaseKehadiran(*m) << "*";
                cout << setw(16) << os.str();
            } else {
                cout << setw(16) << "-";
            }
        }
        if (n && n->isFinal()) cout << setw(8) << fixed << setprecision(2) << n->getSkorAkhir() << KonversiNilai::hurufKeString(n->getNilaiHuruf());
        else cout << setw(8) << "-" << "(belum final)";
        cout << '\n';
    }
    cout << "\n  * = nilai sementara (dihitung otomatis dari tugas/kehadiran saat finalisasi)\n"
         << "  Sebaran nilai: " << ringkasDistribusi(kelas) << '\n';
}

void inputNilaiKomponen(Dosen& dosen, Kelas& kelas) {
    const map<string, double> bobot = kelas.getMataKuliah()->getBobotKomponen();
    vector<string> bisaDiinput;
    cout << "\n  Komponen yang dapat diinput manual:\n";
    for (const auto& [nama, b] : bobot) {
        if (nama == "tugas" || nama == "kehadiran") continue;  // dihitung otomatis
        bisaDiinput.push_back(nama);
        cout << "   " << bisaDiinput.size() << ". " << nama << " (bobot " << b << "%)\n";
    }
    if (bisaDiinput.empty()) { cout << "  Semua komponen dihitung otomatis.\n"; tunggu(); return; }
    const int k = pilihNomor("  Pilih komponen", static_cast<int>(bisaDiinput.size()));
    if (k < 0) return;
    const string komponen = bisaDiinput[k];

    cout << "\n  Masukkan skor 0-100 untuk \"" << komponen << "\" (Enter = lewati peserta).\n";
    int tersimpan = 0, gagal = 0;
    for (Mahasiswa* m : pesertaUrut(kelas)) {
        const Nilai* n = m->getTranskrip().cariNilai(&kelas);
        if (n && n->isFinal()) { cout << "  " << m->getNama() << ": sudah final, dilewati\n"; continue; }
        double skor = 0;
        if (!bacaSkorOpsional("  " + m->getNomorInduk() + " " + pangkas(m->getNama(), 22) + " : ", skor)) continue;
        if (dosen.inputNilai(kelas, *m, komponen, skor)) ++tersimpan;
        else { ++gagal; laporGagal(dosen); }
    }
    cout << "\n  " << tersimpan << " nilai tersimpan, " << gagal << " gagal.\n";
    tunggu();
}

void finalisasi(Dosen& dosen, Kelas& kelas) {
    const map<string, double> bobot = kelas.getMataKuliah()->getBobotKomponen();
    // peringatan: komponen manual yang belum diisi dianggap 0
    int kurang = 0;
    for (const Mahasiswa* m : pesertaUrut(kelas)) {
        const Nilai* n = m->getTranskrip().cariNilai(&kelas);
        if (n && n->isFinal()) continue;
        for (const auto& [nama, b] : bobot) {
            (void)b;
            if (nama == "tugas" || nama == "kehadiran") continue;
            if (!n || n->getKomponen().find(nama) == n->getKomponen().end()) { ++kurang; break; }
        }
    }
    if (kurang > 0) cout << "\n  PERINGATAN: " << kurang << " peserta masih punya komponen kosong; komponen kosong dihitung 0.\n";
    cout << "  Finalisasi mengunci nilai: tidak dapat diubah lagi.\n";
    if (!konfirmasi("  Lanjutkan finalisasi")) { cout << "  Dibatalkan.\n"; tunggu(); return; }
    if (dosen.finalisasiNilai(kelas)) cout << "  Nilai difinalisasi.\n  Sebaran nilai: " << ringkasDistribusi(kelas) << '\n';
    else laporGagal(dosen);
    tunggu();
}

void layarPenilaian(Dosen& dosen, Kelas& kelas) {
    if (kelas.jumlahPeserta() == 0) { cout << "\n  Kelas ini belum memiliki peserta.\n"; tunggu(); return; }
    for (;;) {
        judul("PENILAIAN - " + kelas.getId());
        cout << "  1. Rekap nilai\n  2. Input nilai per komponen\n  3. Finalisasi nilai\n  0. Kembali\n";
        const int p = bacaInt("  Pilihan: ", 0, 3);
        if (p == 0) return;
        if (p == 1) { rekapNilai(kelas); tunggu(); }
        else if (p == 2) inputNilaiKomponen(dosen, kelas);
        else finalisasi(dosen, kelas);
    }
}

// presensi
void layarKehadiran(Dosen& dosen, Kelas& kelas) {
    if (kelas.jumlahPeserta() == 0) { cout << "\n  Kelas ini belum memiliki peserta.\n"; tunggu(); return; }
    for (;;) {
        judul("KEHADIRAN - " + kelas.getId());
        cout << "  1. Catat kehadiran satu pertemuan\n  2. Rekap kehadiran\n  0. Kembali\n";
        const int p = bacaInt("  Pilihan: ", 0, 2);
        if (p == 0) return;

        if (p == 2) {
            const vector<Mahasiswa*> peserta = pesertaUrut(kelas);
            cout << "\n  Pertemuan terlaksana: " << kelas.getDaftarPertemuan().size() << " dari " << kelas.getTotalPertemuan() << '\n';
            cout << "  " << left << setw(12) << "NIU" << setw(24) << "Nama" << "Kehadiran\n";
            garis('-', 50);
            for (const Mahasiswa* m : peserta) {
                cout << "  " << left << setw(12) << m->getNomorInduk() << setw(24) << pangkas(m->getNama(), 23)
                     << fixed << setprecision(1) << kelas.hitungPersentaseKehadiran(*m) << "%\n";
            }
            tunggu();
            continue;
        }

        const int nomor = bacaInt("  Pertemuan ke (1-" + to_string(kelas.getTotalPertemuan()) + ", 0 = batal): ", 0, kelas.getTotalPertemuan());
        if (nomor == 0) continue;
        const string materi = bacaBaris("  Materi (opsional): ");
        cout << "  Untuk tiap peserta: Enter = hadir, t = tidak hadir.\n";
        int hadir = 0, absen = 0;
        for (Mahasiswa* m : pesertaUrut(kelas)) {
            string s = normalisasi(bacaBaris("  " + m->getNomorInduk() + " " + pangkas(m->getNama(), 22) + " : "));
            const bool ya = !(s == "t" || s == "tidak" || s == "n");
            if (dosen.updateKehadiran(kelas, *m, nomor, ya)) (ya ? hadir : absen)++;
            else laporGagal(dosen);
        }
        for (Pertemuan* pt : kelas.getDaftarPertemuan()) {
            if (pt->getNomor() != nomor) continue;
            if (!materi.empty()) pt->setMateri(materi);
            if (pt->getTanggal() == 0) pt->setTanggal(time(nullptr));
        }
        cout << "\n  Pertemuan " << nomor << ": " << hadir << " hadir, " << absen << " tidak hadir.\n";
        tunggu();
    }
}

} 

void menuDosen(SistemAkademik& sistem, Dosen& dosen) {
    dosen.tampilkanDashboard();
    for (;;) {
        dosen.tampilkanMenu();
        const int pilihan = bacaInt("", 0, 6);
        if (pilihan == 0) return;
        if (pilihan == 1) { dosen.tampilkanDashboard(); tunggu(); continue; }
        if (pilihan == 6) { layarGantiPassword(dosen); tunggu(); }
        else if (Kelas* k = pilihKelas(dosen)) {
            switch (pilihan) {
                case 2: layarPengumuman(dosen, *k); break;
                case 3: layarPenugasan(dosen, *k); break;
                case 4: layarPenilaian(dosen, *k); break;
                case 5: layarKehadiran(dosen, *k); break;
            }
        }
        if (!sistem.simpanData()) cout << "  ! PERINGATAN: data gagal disimpan.\n";
    }
}
