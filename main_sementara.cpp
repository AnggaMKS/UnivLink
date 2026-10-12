// Cara build dan run
//build: g++ -std=c++17 -Wall -Wextra -I. main_sementara.cpp jadwal.cpp ruangan.cpp kegiatan.cpp kelas.cpp user.cpp tests/krs_mahasiswa_sementara.cpp -o main_sementara
//run: ./main_sementara       
#include "univlink.h"

#include <iostream>

static void judul(const string& teks) { cout << "\n--- " << teks << " ---\n"; }
static const char* ya(bool b) { return b ? "YA" : "TIDAK"; }

int main() {
    judul("Ruangan dan jadwal");
    Ruangan ruang("R-301", "Gedung DTETI", 40, JenisRuangan::KELAS);
    Kelas kelasA("TI101-A", nullptr, nullptr, "2026/2027-Gasal", 2);
    Kelas kelasB("TI102-A", nullptr, nullptr, "2026/2027-Gasal", 40);

    kelasA.setPenempatan(&ruang, Jadwal(Hari::SENIN, 700, 900));
    cout << "Kelas A dipesan ke R-301 (" << kelasA.getJadwal().toString() << "): " << ya(ruang.pesan(&kelasA)) << "\n";

    kelasB.setPenempatan(&ruang, Jadwal(Hari::SENIN, 800, 1000));
    cout << "Kelas B " << kelasB.getJadwal().toString() << " bentrok dengan A, dipesan? " << ya(ruang.pesan(&kelasB)) << "  (harusnya TIDAK)\n";

    kelasB.setPenempatan(&ruang, Jadwal(Hari::SENIN, 900, 1040));
    cout << "Kelas B digeser ke " << kelasB.getJadwal().toString() << ", dipesan? " << ya(ruang.pesan(&kelasB)) << "  (menempel di ujung = boleh)\n";
    cout << "Apakah A dan B bentrok bagi mahasiswa? " << ya(kelasA.bentrokDengan(kelasB)) << "\n";

    judul("Peserta dan kuota (kuota kelas A = 2)");
    Mahasiswa budi("22/001", "Budi", "budi@mail.ugm.ac.id", "TI", "sandiBudi", 2022, 3, nullptr);
    Mahasiswa sari("22/002", "Sari", "sari@mail.ugm.ac.id", "TI", "sandiSari", 2022, 3, nullptr);
    Mahasiswa joko("22/003", "Joko", "joko@mail.ugm.ac.id", "TI", "sandiJoko", 2022, 3, nullptr);
    KRS krsBudi(&budi, "2026/2027-Gasal"), krsSari(&sari, "2026/2027-Gasal"),
        krsJoko(&joko, "2026/2027-Gasal");

    cout << "Budi daftar: " << ya(krsBudi.tambahKelas(&kelasA)) << "\n";
    cout << "Sari daftar: " << ya(krsSari.tambahKelas(&kelasA)) << "\n";
    cout << "Joko daftar: " << ya(krsJoko.tambahKelas(&kelasA)) << "  (kuota penuh -> TIDAK)\n";
    cout << "Peserta sekarang " << kelasA.jumlahPeserta() << "/" << kelasA.getKuota() << ":";
    for (const Mahasiswa* m : kelasA.getPeserta()) cout << " " << m->getNama();
    cout << "\n";

    judul("Kehadiran");
    kelasA.catatKehadiran(budi, 1, true);
    kelasA.catatKehadiran(sari, 1, false);
    kelasA.catatKehadiran(budi, 2, true);
    cout << "Joko bukan peserta, dicatat? " << ya(kelasA.catatKehadiran(joko, 1, true)) << "\n";
    for (const Pertemuan* p : kelasA.getDaftarPertemuan()) {
        cout << "Pertemuan " << p->getNomor() << ": Budi hadir=" << ya(p->isHadir("22/001")) << ", Sari hadir=" << ya(p->isHadir("22/002")) << "\n";
    }

    judul("Tugas dan pengumpulan");
    const time_t sekarang = time(nullptr);
    auto tugas = make_unique<Tugas>("T1", "Laporan 1", "Kerjakan soal 1-5", sekarang + 86400, 25);
    Tugas* t1 = tugas.get();
    kelasA.tambahTugas(move(tugas));
    cout << "Deadline: " << formatWaktu(t1->getDeadline()) << "\n";
    cout << "Budi kumpul: " << ya(t1->submit(budi, "budi_v1.pdf")) << "\n";
    cout << "Budi kumpul ulang: " << ya(t1->submit(budi, "budi_v2.pdf")) << "  (jumlah pengumpulan tetap " << t1->getDaftarPengumpulan().size() << ")\n";
    t1->getPengumpulan(budi)->beriSkor(88);
    cout << "Skor Budi: " << t1->getPengumpulan(budi)->getSkor() << "\n";

    Tugas lama("T0", "Tugas lama", "", sekarang - 3600, 10);
    cout << "Sari kumpul setelah deadline: " << ya(lama.submit(sari, "telat.pdf")) << "  (harusnya TIDAK)\n";

    judul("Pengumuman");
    auto info = make_unique<Pengumuman>("P1", "UTS dimajukan", "UTS minggu depan", nullptr);
    Pengumuman* p1 = info.get();
    kelasA.tambahPengumuman(move(info));
    cout << "Sebelum diterbitkan, aktif? " << ya(p1->isAktif()) << "\n";
    p1->publikasikan();
    cout << "Setelah diterbitkan, aktif? " << ya(p1->isAktif()) << "  (" << formatWaktu(p1->getTanggal()) << ")\n";
    cout << "Jumlah pengumuman aktif di kelas: " << kelasA.getPengumumanAktif().size() << "\n";

    judul("Kata sandi");
    cout << "Sandi benar : " << ya(budi.verifikasiPassword("sandiBudi")) << "\n";
    cout << "Sandi salah : " << ya(budi.verifikasiPassword("awokawkook")) << "\n";
    cout << "Ganti sandi : " << ya(budi.gantiPassword("sandiBudi", "sandiBaru1")) << "\n";
    cout << "Sandi lama  : " << ya(budi.verifikasiPassword("sandiBudi")) << "  (harusnya TIDAK)\n";
    budi.tampilkanDashboard();

    cout << "\nSelesai.\n";
    return 0;
}