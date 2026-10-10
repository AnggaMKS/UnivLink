//DEKLARASI HEADER FILE

#ifndef UNIVLINK_H
#define UNIVLINK_H

#include <ctime>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std;

class User;
class Mahasiswa;
class Dosen;
class Admin;
class Kurikulum;
class MataKuliah;
class Kelas;
class Ruangan;
class KRS;
class Tugas;
class SistemAkademik;

//enum
enum class NilaiHuruf { A, A_MIN, A_B, B_PLUS, B, B_MIN, B_C, C_PLUS, C, C_MIN, D, E };
enum class StatusKRS { DRAFT, DISAHKAN };
enum class StatusPengumuman { DRAFT, TERBIT };
enum class Hari { SENIN, SELASA, RABU, KAMIS, JUMAT };
enum class JenisRuangan { KELAS, LAB };

string hariKeString(Hari h);
string jenisRuanganKeString(JenisRuangan j);
string formatWaktu(time_t t);

//Penilaian
class KonversiNilai {
public:
    static NilaiHuruf skorKeHuruf(double skor);
    static double hurufKeBobot(NilaiHuruf h);
    static string hurufKeString(NilaiHuruf h);
    static bool isLulus(NilaiHuruf h);  
};

class Nilai {
    Kelas* kelas;                            
    map<string, double> komponen;
    double skorAkhir;                       
    NilaiHuruf nilaiHuruf;                   
    double bobotHuruf;                       
    bool sudahFinal;                        

public:
    explicit Nilai(Kelas* kelas);
    bool updateKomponen(const string& nama, double skor);
    double hitungNilaiAkhir();
    bool finalisasi();
    bool isFinal() const;
    bool isLulus() const;
    NilaiHuruf getNilaiHuruf() const;
    double getBobotHuruf() const;
    double getSkorAkhir() const;
    Kelas* getKelas() const;
    const map<string, double>& getKomponen() const;
};

class Transkrip {
    vector<unique_ptr<Nilai>> listNilai;  
    map<string, const Nilai*> nilaiTerbaikPerMK() const;

public:
    void tambahNilai(Nilai* n);                     
    Nilai* cariNilai(const Kelas* kelas) const;
    Nilai* ambilAtauBuatNilai(Kelas* kelas);
    double hitungIPS(const string& periode) const;
    double hitungIPK() const;
    double hitungIPSTerakhir() const;
    bool sudahLulus(const string& kodeMK) const;
    int totalSKSLulus() const;
    const vector<unique_ptr<Nilai>>& getListNilai() const;
};

//Kelas & kegiatan 
class Jadwal {
    Hari hari;
    int jamMulai;    // format HHMM, mis. 730 = 07:30
    int jamSelesai;  // format HHMM, mis. 1000 = 10:00

public:
    explicit Jadwal(Hari hari = Hari::SENIN, int jamMulai = 0, int jamSelesai = 0);
    Hari getHari() const;
    int getJamMulai() const;
    int getJamSelesai() const;
    bool valid() const;
    bool bentrokDengan(const Jadwal& lain) const;
    string toString() const;
};

class Pertemuan {
    int nomor;
    time_t tanggal;
    string materi;
    map<string, bool> kehadiran;  

public:
    explicit Pertemuan(int nomor);
    int getNomor() const;
    void setMateri(const string& m);;
    void setTanggal(time_t t);
    void catat(const string& niu, bool hadir);
    bool isHadir(const string& niu) const;
};

class Pengumpulan {
    Mahasiswa* pengumpul;  // Pengumpulan "*" --> "1" Mahasiswa : oleh
    string filePath;
    time_t waktuKumpul;
    double skor;
    bool sudahDinilai;

public:
    Pengumpulan(Mahasiswa* pengumpul, const string& filePath);
    void perbarui(const string& fileBaru);
    bool beriSkor(double skor);
    double getSkor() const;
    bool isSudahDinilai() const;
    Mahasiswa* getPengumpul() const;
    const string& getFilePath() const;
    time_t getWaktuKumpul() const;
};

class Tugas {
    string idTugas;
    string judul;
    string deskripsi;
    time_t deadline;
    double bobot;
    vector<unique_ptr<Pengumpulan>> daftarPengumpulan;  // Tugas "1" *-- "*" Pengumpulan

public:
    Tugas(const string& idTugas, const string& judul, const string& deskripsi,
          time_t deadline, double bobot);
    const string& getId() const;
    const string& getJudul() const;
    double getBobot() const;
    time_t getDeadline() const;
    bool submit(Mahasiswa& mhs, const string& filePath);
    bool isDeadlineLewat() const;
    Pengumpulan* getPengumpulan(const Mahasiswa& mhs) const;
};

class Pengumuman {
    string id;
    string judul;
    string isi;
    time_t tanggal;
    StatusPengumuman status;
    Dosen* pembuat;  // Pengumuman "*" --> "1" Dosen : dibuat oleh

public:
    Pengumuman(const string& id, const string& judul, const string& isi, Dosen* pembuat);
    bool edit(const string& isiBaru);
    void publikasikan();
    bool isAktif() const;  // TERBIT dan umurnya <= 30 hari
    const string& getId() const;
    const string& getJudul() const;
    const string& getIsi() const;
    Dosen* getPembuat() const;
    StatusPengumuman getStatus() const;
};

class Ruangan {
    string kodeRuangan;
    string gedung;
    int kapasitas;
    JenisRuangan jenis;
    vector<Kelas*> terjadwal;

public:
    Ruangan(const string& kode, const string& gedung, int kapasitas, JenisRuangan jenis);
    const string& getKode() const;
    const string& getGedung() const;
    int getKapasitas() const;
    JenisRuangan getJenis() const;
    // periode kosong = bandingkan dengan semua kelas, apa pun periodenya
    bool tersedia(const Jadwal& j, const string& periode = "") const;
    bool pesan(Kelas* kelas);
    void lepas(Kelas* kelas);
};

class KRS {
    Mahasiswa* pemilik;
    string periode;
    StatusKRS status;
    vector<Kelas*> daftarKelas;  // KRS "*" -- "1..*" Kelas : memuat

public:
    KRS(Mahasiswa* pemilik, const string& periode);
    const string& getPeriode() const;
    StatusKRS getStatus() const;
    Mahasiswa* getPemilik() const;
    const vector<Kelas*>& getDaftarKelas() const;
    bool memuat(const Kelas* kelas) const;

    bool tambahKelas(Kelas* kelas);
    bool hapusKelas(Kelas* kelas);
    int hitungTotalSKS() const;
    bool validasi(const Kelas& kelas, string& alasan) const;
    bool sahkan();

    // dipakai penyimpanan saat memuat data (tanpa validasi)
    bool pulihkan(Kelas* kelas);
    void pulihkanStatus(StatusKRS s);
};

class Kelas {
    string idKelas;
    string periode;
    int kuota;
    int totalPertemuan;
    MataKuliah* mataKuliah;  // Kelas "*" --> "1" MataKuliah : instance dari
    Dosen* dosen;            // Kelas "*" -- "1" Dosen : diampu oleh
    Ruangan* ruangan;        // Kelas "*" -- "1" Ruangan : berlangsung di
    Jadwal jadwal;           // Kelas "1" *-- "1" Jadwal
    bool jadwalTerisi;
    vector<KRS*> daftarKRS;
    vector<unique_ptr<Tugas>> daftarTugas;
    vector<unique_ptr<Pertemuan>> daftarPertemuan;
    vector<unique_ptr<Pengumuman>> daftarPengumuman;

public:
    Kelas(const string& idKelas, MataKuliah* mk, Dosen* dosen, const string& periode,
          int kuota, int totalPertemuan = 14);

    const string& getId() const;
    const string& getPeriode() const;
    int getKuota() const;
    int getTotalPertemuan() const;
    MataKuliah* getMataKuliah() const;
    Dosen* getDosen() const;
    Ruangan* getRuangan() const;
    const Jadwal& getJadwal() const;
    bool punyaJadwal() const;
    void setPenempatan(Ruangan* r, const Jadwal& j);

    int jumlahPeserta() const;
    bool masihAdaKuota() const;
    bool daftarkan(KRS* krs);
    bool keluarkan(KRS* krs);
    vector<Mahasiswa*> getPeserta() const;
    bool memilikiPeserta(const Mahasiswa& mhs) const;

    void tambahTugas(Tugas* t);              // Kelas mengambil alih kepemilikan
    void tambahPengumuman(Pengumuman* p);    // Kelas mengambil alih kepemilikan
    vector<Tugas*> getDaftarTugas() const;
    vector<Pengumuman*> getDaftarPengumuman() const;
    bool memilikiTugas(const Tugas* t) const;

    bool catatKehadiran(Mahasiswa& mhs, int pertemuanKe, bool hadir);
    double hitungPersentaseKehadiran(const Mahasiswa& mhs) const;
    double hitungNilaiTugas(const Mahasiswa& mhs) const;
    map<NilaiHuruf, int> hitungDistribusiNilai() const;
    vector<Pengumuman*> getPengumumanAktif() const;
    bool bentrokDengan(const Kelas& lain) const;
};

//Kurikulum
class MataKuliah {
protected:
    string kodeMK;
    string namaMK;
    int sks;
    int semester;
    map<string, double> bobotKomponen;  // total bobot = 100
    double hitungRataRataBerbobot(const map<string, double>& komponen) const;

private:
    vector<MataKuliah*> prasyarat;  // MataKuliah "*" --> "0..*" MataKuliah

public:
    MataKuliah(const string& kode, const string& nama, int sks, int semester);
    virtual ~MataKuliah() = default;

    string getKode() const;
    string getNama() const;
    int getSKS() const;
    int getSemester() const;
    vector<MataKuliah*> getPrasyarat() const;
    bool tambahPrasyarat(MataKuliah* mk);          // menolak self-reference & siklus
    bool membutuhkan(const MataKuliah* target) const;  // prasyarat langsung/tidak langsung?
    map<string, double> getBobotKomponen() const;
    bool setBobotKomponen(const map<string, double>& bobotBaru);  // total 100

    virtual JenisRuangan getJenisRuangan() const = 0;
    virtual double hitungNilaiAkhir(const map<string, double>& komponen) const = 0;
};

class MataKuliahTeori : public MataKuliah {
public:
    MataKuliahTeori(const string& kode, const string& nama, int sks, int semester);
    JenisRuangan getJenisRuangan() const override;
    double hitungNilaiAkhir(const map<string, double>& komponen) const override;
};

class MataKuliahPraktikum : public MataKuliah {
public:
    MataKuliahPraktikum(const string& kode, const string& nama, int sks, int semester);
    JenisRuangan getJenisRuangan() const override;
    double hitungNilaiAkhir(const map<string, double>& komponen) const override;
};

class Kurikulum {
    string prodi;
    int tahun;
    map<int, vector<MataKuliah*>> petaSemester;  // agregasi: tidak memiliki MataKuliah

public:
    Kurikulum(const string& prodi, int tahun);
    const string& getProdi() const;
    int getTahun() const;
    bool tambahMataKuliah(MataKuliah* mk);
    MataKuliah* cariMataKuliah(const string& kode) const;
    vector<MataKuliah*> cariMataKuliah(int semester) const;
    vector<MataKuliah*> semuaMataKuliah() const;
    bool validasiGrafPrasyarat() const;  // tanpa siklus & prasyarat ada di kurikulum
};



//User
class User {
protected:
    string nomorInduk;
    string nama;
    string email;
    string prodi;
    void cetakHeader() const;

private:
    string passwordHash;
    static string hashPassword(const string& password);

public:
    User(string nomorInduk, string nama, string email, string prodi,
         const string& password, bool passwordSudahHash = false);
    virtual ~User() = default;

    string getNomorInduk() const;
    string getNama() const;
    string getEmail() const;
    string getProdi() const;
    string getPasswordHash() const;
    bool verifikasiPassword(const string& password) const;
    bool gantiPassword(const string& lama, const string& baru);

    virtual string getPeran() const = 0;
    virtual void tampilkanDashboard() const = 0;
    virtual void tampilkanMenu() const = 0;
};

class Mahasiswa : public User {
    int angkatan;
    int semesterBerjalan;
    Kurikulum* kurikulum;                          // Mahasiswa "*" --> "1" Kurikulum
    vector<unique_ptr<KRS>> daftarKRS;             // Mahasiswa "1" *-- "0..*" KRS
    Transkrip transkrip;                           // Mahasiswa "1" *-- "1" Transkrip

public:
    Mahasiswa(string nomorInduk, string nama, string email, string prodi,
              const string& password, int angkatan, int semesterBerjalan, Kurikulum* kurikulum,
              bool passwordSudahHash = false);

    int getAngkatan() const;
    int getSemesterBerjalan() const;
    Kurikulum* getKurikulum() const;
    Transkrip& getTranskrip();
    const Transkrip& getTranskrip() const;
    const vector<unique_ptr<KRS>>& getDaftarKRS() const;
    KRS* cariKRS(const string& periode) const;

    KRS* buatKRS(const string& periode);
    bool ambilKelas(Kelas& kelas);
    bool batalKelas(Kelas& kelas);
    int hitungMaxSKS() const;
    bool cekPrasyarat(const MataKuliah& mk) const;
    double getIPK() const;            // ipk (turunan dari Transkrip)
    double getIPSTerakhir() const;    // ipsTerakhir (turunan dari Transkrip)
    void lihatTranskrip() const;
    void lihatDetailKelas(const Kelas& kelas) const;
    bool submitTugas(Tugas& tugas, const string& file);
    vector<Kelas*> getJadwalSemesterIni() const;
    vector<Pengumuman*> getPengumumanAktif() const;

    string getPeran() const override;
    void tampilkanDashboard() const override;
    void tampilkanMenu() const override;
};

class Dosen : public User {
    vector<Kelas*> kelasDiampu;
    bool mengampu(const Kelas& kelas) const;

public:
    using User::User;
    void tambahKelasDiampu(Kelas* kelas);
    vector<Kelas*> getKelasDiampu() const;

    Pengumuman* buatPengumuman(Kelas& kelas, const string& judul, const string& isi);
    bool editPengumuman(Pengumuman& p, const string& isiBaru);
    Tugas* buatTugas(Kelas& kelas, const string& judul, const string& deskripsi,
                     time_t deadline, double bobot);
    bool beriSkorTugas(Tugas& tugas, Mahasiswa& mhs, double skor);
    bool inputNilai(Kelas& kelas, Mahasiswa& mhs, const string& komponen, double skor);
    bool updateKehadiran(Kelas& kelas, Mahasiswa& mhs, int pertemuanKe, bool hadir);
    bool finalisasiNilai(Kelas& kelas);

    string getPeran() const override;
    void tampilkanDashboard() const override;
    void tampilkanMenu() const override;
};

class Admin : public User {
    SistemAkademik* sistem;

public:
    Admin(string nomorInduk, string nama, string email, string prodi,
          const string& password, SistemAkademik* sistem, bool passwordSudahHash = false);

    // Ruangan* diambil alih SistemAkademik (jika ditolak/duplikat, objek dihapus)
    bool tambahRuangan(Ruangan* r);
    Kelas* buatKelas(MataKuliah& mk, Dosen& dosen, const string& periode, int kuota);
    bool assignRuangan(Kelas& kelas, Ruangan& r, const Jadwal& j);

    string getPeran() const override;
    void tampilkanDashboard() const override;
    void tampilkanMenu() const override;
};

//Sistem 
class IPenyimpanan {
public:
    virtual ~IPenyimpanan() = default;
    virtual bool muat(SistemAkademik& sistem) = 0;
    virtual bool simpan(SistemAkademik& sistem) = 0;
};

class SistemAkademik {
    User* userAktif;
    vector<unique_ptr<User>> daftarUser;            // *-- "*" User
    vector<unique_ptr<Kelas>> daftarKelas;          // *-- "*" Kelas
    vector<unique_ptr<Ruangan>> daftarRuangan;      // *-- "*" Ruangan
    vector<unique_ptr<Kurikulum>> daftarKurikulum;  // *-- "1..*" Kurikulum
    IPenyimpanan* penyimpanan;                      // o-- "1" IPenyimpanan

public:
    explicit SistemAkademik(IPenyimpanan* penyimpanan = nullptr);
    void setPenyimpanan(IPenyimpanan* p);

    User* login(const string& nomorInduk, const string& password);
    void logout();
    User* getUserAktif() const;

    User* cariUser(const string& nomorInduk) const;
    Kelas* cariKelas(const string& idKelas) const;
    Ruangan* cariRuangan(const string& kode) const;
    MataKuliah* cariMataKuliah(const string& kode) const;
    Kurikulum* getKurikulum(const string& prodi) const;  // tahun terbaru

    // mengembalikan nullptr jika duplikat (objek yang diserahkan ikut dihapus)
    User* tambahUser(unique_ptr<User> user);
    Kelas* tambahKelas(unique_ptr<Kelas> kelas);
    Ruangan* tambahRuangan(unique_ptr<Ruangan> ruangan);
    Kurikulum* tambahKurikulum(unique_ptr<Kurikulum> kurikulum);

    const vector<unique_ptr<User>>& getDaftarUser() const;
    const vector<unique_ptr<Kelas>>& getDaftarKelas() const;
    const vector<unique_ptr<Ruangan>>& getDaftarRuangan() const;
    const vector<unique_ptr<Kurikulum>>& getDaftarKurikulum() const;

    bool muatData();
    bool simpanData();
};

class PenyimpananFile : public IPenyimpanan {
    string folderData;

public:
    explicit PenyimpananFile(string folderData = "data");
    bool muat(SistemAkademik& sistem) override;
    bool simpan(SistemAkademik& sistem) override;
};

#endif  // UNIVLINK_H
