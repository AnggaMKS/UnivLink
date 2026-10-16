#include "univlink.h"
#include <ctime>

string formatWaktu(time_t t) {
    if (t <= 0) return "-";
    const tm* info = localtime(&t);
    if (!info) return "-";
    char buf[32];
    if (strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M", info) == 0) return "-";
    return string(buf);
}
Pertemuan::Pertemuan(int nomor) : nomor(nomor), tanggal(0) {}

int Pertemuan::getNomor() const { return nomor; }
void Pertemuan::setMateri(const string& m) { materi = m; }
void Pertemuan::setTanggal(time_t t) { tanggal = t; }
time_t Pertemuan::getTanggal() const { return tanggal; }
const string& Pertemuan::getMateri() const { return materi; }
const map<string, bool>& Pertemuan::getKehadiran() const { return kehadiran; }
void Pertemuan::catat(const string& niu, bool hadir) { kehadiran[niu] = hadir; }

bool Pertemuan::isHadir(const string& niu) const {
    auto it = kehadiran.find(niu);
    return it != kehadiran.end() && it->second;
}

namespace {
    bool skorSah(double s) { return s >= 0.0 && s <= 100.0; }
}

Pengumpulan::Pengumpulan(Mahasiswa* pengumpul, const string& filePath) : pengumpul(pengumpul), filePath(filePath), waktuKumpul(time(nullptr)), skor(0.0), sudahDinilai(false) {}
void Pengumpulan::perbarui(const string& fileBaru) {
    filePath = fileBaru;
    waktuKumpul = time(nullptr);
    skor = 0.0;
    sudahDinilai = false;
}

bool Pengumpulan::beriSkor(double nilaiBaru) {
    if (!skorSah(nilaiBaru)) return false;
    skor = nilaiBaru;
    sudahDinilai = true;
    return true;
}

double Pengumpulan::getSkor() const { return skor; }
bool Pengumpulan::isSudahDinilai() const { return sudahDinilai; }
Mahasiswa* Pengumpulan::getPengumpul() const { return pengumpul; }
const string& Pengumpulan::getFilePath() const { return filePath; }
time_t Pengumpulan::getWaktuKumpul() const { return waktuKumpul; }

void Pengumpulan::pulihkan(time_t waktu, double skorTersimpan, bool dinilai) {
    waktuKumpul = waktu;
    skor = skorTersimpan;
    sudahDinilai = dinilai;
}

Tugas::Tugas(const string& idTugas, const string& judul, const string& deskripsi,time_t deadline, double bobot) : idTugas(idTugas), judul(judul), deskripsi(deskripsi), deadline(deadline), bobot(bobot < 0.0 ? 0.0 : bobot) {}

const string& Tugas::getId() const { return idTugas; }
const string& Tugas::getJudul() const { return judul; }
const string& Tugas::getDeskripsi() const { return deskripsi; }
double Tugas::getBobot() const { return bobot; }
time_t Tugas::getDeadline() const { return deadline; }

bool Tugas::isDeadlineLewat() const { return time(nullptr) > deadline; }


// Aturan submit:
//   1. path file tidak boleh kosong
//   2. tidak boleh lewat deadline (sesuai spesifikasi: portal hanya menerima
//      pengumpulan SEBELUM tenggat)
//   3. sudah pernah mengumpul -> file lama DIGANTI (perbarui), bukan ditambah
//      sehingga satu mahasiswa selalu punya paling banyak satu Pengumpulan
//

bool Tugas::submit(Mahasiswa& mhs, const string& filePath) {
    if (filePath.empty()) return false;
    if (isDeadlineLewat()) return false;

    if (Pengumpulan* ada = getPengumpulan(mhs)) {
        ada->perbarui(filePath);
        return true;
    }
    daftarPengumpulan.push_back(make_unique<Pengumpulan>(&mhs, filePath));
    return true;
}

Pengumpulan* Tugas::getPengumpulan(const Mahasiswa& mhs) const {
    for (const auto& p : daftarPengumpulan) {
        if (p->getPengumpul() &&
            p->getPengumpul()->getNomorInduk() == mhs.getNomorInduk()) {
            return p.get();
        }
    }
    return nullptr;
}

vector<Pengumpulan*> Tugas::getDaftarPengumpulan() const {
    vector<Pengumpulan*> hasil;
    hasil.reserve(daftarPengumpulan.size());
    for (const auto& p : daftarPengumpulan) hasil.push_back(p.get());
    return hasil;
}

Pengumpulan* Tugas::pulihkanPengumpulan(Mahasiswa* mhs, const string& file) {
    if (!mhs) return nullptr;
    if (Pengumpulan* ada = getPengumpulan(*mhs)) {
        ada->perbarui(file);
        return ada;
    }
    daftarPengumpulan.push_back(make_unique<Pengumpulan>(mhs, file));
    return daftarPengumpulan.back().get();
}

namespace {
    constexpr double BATAS_AKTIF_DETIK = 30.0 * 24.0 * 60.0 * 60.0;
}

Pengumuman::Pengumuman(const string& id, const string& judul, const string& isi, Dosen* pembuat) : id(id), judul(judul), isi(isi), tanggal(time(nullptr)), status(StatusPengumuman::DRAFT), pembuat(pembuat) {}

bool Pengumuman::edit(const string& isiBaru) {
    if (isiBaru.empty()) return false;
    isi = isiBaru;
    return true;
}

void Pengumuman::publikasikan() {
    status = StatusPengumuman::TERBIT;
    tanggal = time(nullptr);
}

bool Pengumuman::isAktif() const {
    if (status != StatusPengumuman::TERBIT) return false;
    return difftime(time(nullptr), tanggal) <= BATAS_AKTIF_DETIK;
}

const string& Pengumuman::getId() const { return id; }
const string& Pengumuman::getJudul() const { return judul; }
const string& Pengumuman::getIsi() const { return isi; }
Dosen* Pengumuman::getPembuat() const { return pembuat; }
StatusPengumuman Pengumuman::getStatus() const { return status; }
time_t Pengumuman::getTanggal() const { return tanggal; }

void Pengumuman::pulihkan(time_t tanggalTersimpan, StatusPengumuman statusTersimpan) {
    tanggal = tanggalTersimpan;
    status = statusTersimpan;
}
