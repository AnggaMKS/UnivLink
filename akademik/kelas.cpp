#include "univlink.h"
#include <algorithm>

namespace {
    Pertemuan* ambilAtauBuatPertemuan(vector<unique_ptr<Pertemuan>>& daftar, int nomor) {
        auto it = lower_bound(daftar.begin(), daftar.end(), nomor, [](const unique_ptr<Pertemuan>& p, int n) { return p->getNomor() < n; });
        if (it != daftar.end() && (*it)->getNomor() == nomor) return it->get();
        it = daftar.insert(it, make_unique<Pertemuan>(nomor));
        return it->get();
    }
}  
Kelas::Kelas(const string& idKelas, MataKuliah* mk, Dosen* dosen, const string& periode, int kuota, int totalPertemuan)
    : idKelas(idKelas),
      periode(periode),
      kuota(kuota < 0 ? 0 : kuota),
      totalPertemuan(totalPertemuan > 0 ? totalPertemuan : 14),
      mataKuliah(mk),
      dosen(dosen),
      ruangan(nullptr),
      jadwal(),
      jadwalTerisi(false) {}

const string& Kelas::getId() const { return idKelas; }
const string& Kelas::getPeriode() const { return periode; }
int Kelas::getKuota() const { return kuota; }
int Kelas::getTotalPertemuan() const { return totalPertemuan; }
MataKuliah* Kelas::getMataKuliah() const { return mataKuliah; }
Dosen* Kelas::getDosen() const { return dosen; }
Ruangan* Kelas::getRuangan() const { return ruangan; }
const Jadwal& Kelas::getJadwal() const { return jadwal; }
bool Kelas::punyaJadwal() const { return jadwalTerisi && jadwal.valid(); }

void Kelas::setPenempatan(Ruangan* r, const Jadwal& j) {
    ruangan = r;
    jadwal = j;
    jadwalTerisi = true;
}

int Kelas::jumlahPeserta() const { return static_cast<int>(daftarKRS.size()); }
bool Kelas::masihAdaKuota() const { return jumlahPeserta() < kuota; }

bool Kelas::daftarkan(KRS* krs) {
    if (!krs) return false;
    if (find(daftarKRS.begin(), daftarKRS.end(), krs) != daftarKRS.end()) return false;
    if (!masihAdaKuota()) return false;
    daftarKRS.push_back(krs);
    return true;
}

bool Kelas::keluarkan(KRS* krs) {
    auto it = remove(daftarKRS.begin(), daftarKRS.end(), krs);
    if (it == daftarKRS.end()) return false;
    daftarKRS.erase(it, daftarKRS.end());
    return true;
}

vector<Mahasiswa*> Kelas::getPeserta() const {
    vector<Mahasiswa*> hasil;
    hasil.reserve(daftarKRS.size());
    for (const KRS* krs : daftarKRS) {
        if (krs && krs->getPemilik()) hasil.push_back(krs->getPemilik());
    }
    return hasil;
}

bool Kelas::memilikiPeserta(const Mahasiswa& mhs) const {
    for (const KRS* krs : daftarKRS) {
        if (krs && krs->getPemilik() == &mhs) return true;
    }
    return false;
}

void Kelas::tambahTugas(unique_ptr<Tugas> t) {
    if (!t) return;
    for (const auto& ada : daftarTugas) {
        if (ada->getId() == t->getId()) return;
    }
    daftarTugas.push_back(move(t));
}

vector<Tugas*> Kelas::getDaftarTugas() const {
    vector<Tugas*> hasil;
    hasil.reserve(daftarTugas.size());
    for (const auto& t : daftarTugas) hasil.push_back(t.get());
    return hasil;
}

bool Kelas::memilikiTugas(const Tugas* t) const {
    for (const auto& ada : daftarTugas) {
        if (ada.get() == t) return true;
    }
    return false;
}

void Kelas::tambahPengumuman(unique_ptr<Pengumuman> p) {
    if (!p) return;
    for (const auto& ada : daftarPengumuman) {
        if (ada->getId() == p->getId()) return;
    }
    daftarPengumuman.push_back(move(p));
}

vector<Pengumuman*> Kelas::getDaftarPengumuman() const {
    vector<Pengumuman*> hasil;
    hasil.reserve(daftarPengumuman.size());
    for (const auto& p : daftarPengumuman) hasil.push_back(p.get());
    return hasil;
}

vector<Pengumuman*> Kelas::getPengumumanAktif() const {
    vector<Pengumuman*> hasil;
    for (const auto& p : daftarPengumuman) {
        if (p->isAktif()) hasil.push_back(p.get());
    }
    sort(hasil.begin(), hasil.end(),
         [](const Pengumuman* a, const Pengumuman* b) { return a->getTanggal() > b->getTanggal(); });
    return hasil;
}

bool Kelas::catatKehadiran(Mahasiswa& mhs, int pertemuanKe, bool hadir) {
    if (pertemuanKe < 1 || pertemuanKe > totalPertemuan) return false;
    if (!memilikiPeserta(mhs)) return false;

    Pertemuan* p = ambilAtauBuatPertemuan(daftarPertemuan, pertemuanKe);
    p->catat(mhs.getNomorInduk(), hadir);
    return true;
}

vector<Pertemuan*> Kelas::getDaftarPertemuan() const {
    vector<Pertemuan*> hasil;
    hasil.reserve(daftarPertemuan.size());
    for (const auto& p : daftarPertemuan) hasil.push_back(p.get());
    return hasil;
}

bool Kelas::bentrokDengan(const Kelas& lain) const {
    if (this == &lain) return false;
    if (periode != lain.periode) return false;
    if (!punyaJadwal() || !lain.punyaJadwal()) return false;
    return jadwal.bentrokDengan(lain.jadwal);
}
