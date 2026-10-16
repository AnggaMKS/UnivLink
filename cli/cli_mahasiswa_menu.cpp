#include "cli.h"
#include <algorithm>
#include <iomanip>
using namespace cli;

namespace {

// Kelas pada KRS mahasiswa (semua periode), terbaru dulu.
vector<Kelas*> kelasPadaKRS(const Mahasiswa& m) {
    vector<Kelas*> hasil;
    for (const auto& krs : m.getDaftarKRS()) {
        for (Kelas* k : krs->getDaftarKelas()) hasil.push_back(k);
    }
    sort(hasil.begin(), hasil.end(), [](const Kelas* a, const Kelas* b) {
        if (a->getPeriode() != b->getPeriode()) return a->getPeriode() > b->getPeriode();
        return a->getId() < b->getId();
    });
    return hasil;
}

// daftar krs
void layarRegistrasi(SistemAkademik& sistem, Mahasiswa& m) {
    const string periode = periodeTerbaru(sistem);
    if (periode.empty()) {
        cout << "\n  Belum ada kelas yang dibuka.\n";
        tunggu();
        return;
    }
    KRS* krs = m.cariKRS(periode);
    if (!krs) krs = m.buatKRS(periode);
    if (!krs) { cout << "  ! Gagal membuat KRS.\n"; tunggu(); return; }

    for (;;) {
        judul("REGISTRASI KRS - " + periode);
        cout << "  Status KRS : " << (krs->getStatus() == StatusKRS::DISAHKAN ? "DISAHKAN" : "DRAFT") << '\n'
             << "  SKS diambil: " << krs->hitungTotalSKS() << " dari maksimum " << m.hitungMaxSKS()
             << " (IPS terakhir " << fixed << setprecision(2) << m.getIPSTerakhir() << ")\n\n";

        // kelas periode ini yang mata kuliahnya ada di kurikulum mahasiswa
        vector<Kelas*> daftar;
        for (const auto& k : sistem.getDaftarKelas()) {
            if (k->getPeriode() != periode || !k->getMataKuliah()) continue;
            if (m.getKurikulum() && m.getKurikulum()->cariMataKuliah(k->getMataKuliah()->getKode()) != k->getMataKuliah()) continue;
            daftar.push_back(k.get());
        }
        sort(daftar.begin(), daftar.end(), [](const Kelas* a, const Kelas* b) {
            if (a->getMataKuliah()->getSemester() != b->getMataKuliah()->getSemester())
                return a->getMataKuliah()->getSemester() < b->getMataKuliah()->getSemester();
            return a->getId() < b->getId();
        });
        if (daftar.empty()) { cout << "  Tidak ada kelas yang tersedia untuk kurikulum Anda.\n"; tunggu(); return; }

        cout << "  " << left << setw(4) << "No" << setw(15) << "Kelas" << setw(26) << "Mata Kuliah" << setw(5) << "Smt"
             << setw(5) << "SKS" << setw(20) << "Jadwal" << setw(8) << "Kuota" << "Status\n";
        garis('-', 104);
        for (size_t i = 0; i < daftar.size(); ++i) {
            const Kelas* k = daftar[i];
            string status;
            if (krs->memuat(k)) status = "[DIAMBIL]";
            else {
                string alasan;
                status = krs->validasi(*k, alasan) ? "tersedia" : pangkas(alasan, 40);
            }
            cout << "  " << left << setw(4) << (i + 1) << setw(15) << pangkas(k->getId(), 14)
                 << setw(26) << pangkas(k->getMataKuliah()->getNama(), 25) << setw(5) << k->getMataKuliah()->getSemester()
                 << setw(5) << k->getMataKuliah()->getSKS() << setw(20) << (k->punyaJadwal() ? k->getJadwal().toString() : "-")
                 << setw(8) << (to_string(k->jumlahPeserta()) + "/" + to_string(k->getKuota())) << status << '\n';
        }

        const int idx = pilihNomor("\n  Pilih nomor kelas untuk diambil", static_cast<int>(daftar.size()));
        if (idx < 0) return;

        Kelas* pilihan = daftar[idx];
        string alasan;
        if (!krs->validasi(*pilihan, alasan)) {
            cout << "\n  DITOLAK: " << alasan << '\n';
        } else if (m.ambilKelas(*pilihan)) {
            cout << "\n  Berhasil: " << pilihan->getMataKuliah()->getNama() << " (" << pilihan->getId() << ") ditambahkan ke KRS.\n";
        } else {
            cout << "\n  ! Gagal menambahkan kelas.\n";
        }
        tunggu();
    }
}

// lihat atau batal krs
void layarKRS(SistemAkademik& sistem, Mahasiswa& m) {
    for (;;) {
        judul("KARTU RENCANA STUDI (KRS)");
        if (m.getDaftarKRS().empty()) { cout << "  Anda belum memiliki KRS.\n"; tunggu(); return; }

        for (const auto& krs : m.getDaftarKRS()) {
            cout << "  Periode " << krs->getPeriode() << "  |  "
                 << (krs->getStatus() == StatusKRS::DISAHKAN ? "DISAHKAN" : "DRAFT")
                 << "  |  " << krs->hitungTotalSKS() << " SKS\n";
            if (krs->getDaftarKelas().empty()) cout << "     (kosong)\n";
            for (const Kelas* k : krs->getDaftarKelas()) {
                cout << "     - " << left << setw(17) << pangkas(k->getId(), 16) << setw(28) << pangkas(k->getMataKuliah()->getNama(), 27)
                     << setw(4) << k->getMataKuliah()->getSKS() << (k->punyaJadwal() ? k->getJadwal().toString() : "-")
                     << "  " << (k->getRuangan() ? k->getRuangan()->getKode() : "") << '\n';
            }
            cout << '\n';
        }

        const string periode = periodeTerbaru(sistem);
        KRS* aktif = m.cariKRS(periode);
        if (!aktif || aktif->getStatus() == StatusKRS::DISAHKAN) {
            cout << "  (KRS periode " << periode << " sudah disahkan atau belum dibuat: tidak dapat diubah)\n";
            tunggu();
            return;
        }
        cout << "  1. Batalkan salah satu kelas (" << periode << ")\n"
             << "  2. Sahkan KRS " << periode << " (setelah disahkan tidak dapat diubah)\n"
             << "  0. Kembali\n";
        const int p = bacaInt("  Pilihan: ", 0, 2);
        if (p == 0) return;

        if (p == 1) {
            const vector<Kelas*>& kelas = aktif->getDaftarKelas();
            if (kelas.empty()) { cout << "  KRS masih kosong.\n"; tunggu(); continue; }
            for (size_t i = 0; i < kelas.size(); ++i)
                cout << "   " << (i + 1) << ". " << kelas[i]->getMataKuliah()->getNama() << " (" << kelas[i]->getId() << ")\n";
            const int idx = pilihNomor("  Kelas yang dibatalkan", static_cast<int>(kelas.size()));
            if (idx < 0) continue;
            Kelas* target = kelas[idx];  // salin pointer: daftar akan berubah setelah hapus
            if (m.batalKelas(*target)) cout << "  Kelas dibatalkan.\n";
            else cout << "  ! Gagal membatalkan kelas.\n";
        } else {
            if (aktif->getDaftarKelas().empty()) cout << "  ! KRS kosong tidak dapat disahkan.\n";
            else if (!konfirmasi("  Yakin mengesahkan KRS? Tindakan ini tidak dapat dibatalkan")) cout << "  Dibatalkan.\n";
            else if (aktif->sahkan()) cout << "  KRS disahkan.\n";
            else cout << "  ! KRS tidak dapat disahkan (periksa batas SKS).\n";
        }
        tunggu();
    }
}

// detail portal dan tugas
void layarDetailKelas(Mahasiswa& m) {
    vector<Kelas*> kelas = kelasPadaKRS(m);
    if (kelas.empty()) { cout << "\n  Anda belum mengambil kelas.\n"; tunggu(); return; }

    judul("PILIH KELAS");
    for (size_t i = 0; i < kelas.size(); ++i)
        cout << "   " << (i + 1) << ". [" << kelas[i]->getPeriode() << "] " << kelas[i]->getMataKuliah()->getNama()
             << " (" << kelas[i]->getId() << ")\n";
    const int idx = pilihNomor("\n  Pilih kelas", static_cast<int>(kelas.size()));
    if (idx < 0) return;
    Kelas* k = kelas[idx];

    for (;;) {
        m.lihatDetailKelas(*k);
        cout << "  1. Kumpulkan tugas\n  0. Kembali\n";
        if (bacaInt("  Pilihan: ", 0, 1) == 0) return;

        const vector<Tugas*> tugas = k->getDaftarTugas();
        if (tugas.empty()) { cout << "  Belum ada tugas.\n"; tunggu(); continue; }
        for (size_t i = 0; i < tugas.size(); ++i)
            cout << "   " << (i + 1) << ". " << tugas[i]->getJudul() << "  (tenggat " << formatWaktu(tugas[i]->getDeadline()) << ")\n";
        const int t = pilihNomor("  Pilih tugas", static_cast<int>(tugas.size()));
        if (t < 0) continue;

        if (tugas[t]->isDeadlineLewat()) { cout << "  ! Tenggat sudah lewat, tugas tidak dapat dikumpulkan.\n"; tunggu(); continue; }
        if (!k->memilikiTugas(tugas[t]) || !k->memilikiPeserta(m)) { cout << "  ! Anda tidak terdaftar pada kelas ini.\n"; tunggu(); continue; }
        const string berkas = bacaBaris("  Nama/jalur berkas (kosong = batal): ");
        if (berkas.empty()) continue;
        if (m.submitTugas(*tugas[t], berkas)) cout << "  Tugas berhasil dikumpulkan (mengumpulkan ulang sebelum tenggat akan menggantikan berkas lama).\n";
        else cout << "  ! Gagal mengumpulkan tugas.\n";
        tunggu();
    }
}

void layarPengumuman(Mahasiswa& m) {
    judul("PENGUMUMAN");
    bool ada = false;
    for (const Kelas* k : m.getJadwalSemesterIni()) {
        for (const Pengumuman* p : k->getPengumumanAktif()) {
            ada = true;
            cout << "  [" << k->getMataKuliah()->getNama() << "]  " << formatWaktu(p->getTanggal()) << '\n'
                 << "  " << p->getJudul() << '\n'
                 << "    " << p->getIsi() << "\n\n";
        }
    }
    if (!ada) cout << "  (Tidak ada pengumuman aktif)\n";
    tunggu();
}

}

void menuMahasiswa(SistemAkademik& sistem, Mahasiswa& m) {
    m.tampilkanDashboard();
    for (;;) {
        m.tampilkanMenu();
        const int pilihan = bacaInt("", 0, 7);
        switch (pilihan) {
            case 1: m.tampilkanDashboard(); tunggu(); break;
            case 2: layarRegistrasi(sistem, m); break;
            case 3: layarKRS(sistem, m); break;
            case 4: m.lihatTranskrip(); tunggu(); break;
            case 5: layarDetailKelas(m); break;
            case 6: layarPengumuman(m); break;
            case 7: layarGantiPassword(m); tunggu(); break;
            case 0: return;
        }
        if (!sistem.simpanData()) cout << "  ! PERINGATAN: data gagal disimpan.\n";
    }
}
