// pemilik SEMUA objek utama (User, MataKuliah, Kurikulum, Ruangan,
// Kelas) dan pintu masuk login. Class lain hanya menyimpan pointer tanpa
// kepemilikan ke objek-objek ini; selama SistemAkademik hidup, pointer itu aman.

#include "univlink.h"

SistemAkademik::SistemAkademik(IPenyimpanan* penyimpanan)
    : userAktif(nullptr), penyimpanan(penyimpanan) {}

void SistemAkademik::setPenyimpanan(IPenyimpanan* p) { penyimpanan = p; }

// login: nomor induk harus ada DAN kata sandi cocok.
User* SistemAkademik::login(const string& nomorInduk, const string& password) {
    User* u = cariUser(nomorInduk);
    if (!u || !u->verifikasiPassword(password)) return nullptr;
    userAktif = u;
    return u;
}

void SistemAkademik::logout() { userAktif = nullptr; }
User* SistemAkademik::getUserAktif() const { return userAktif; }

User* SistemAkademik::cariUser(const string& nomorInduk) const {
    for (const auto& u : daftarUser) {
        if (u->getNomorInduk() == nomorInduk) return u.get();
    }
    return nullptr;
}

Kelas* SistemAkademik::cariKelas(const string& idKelas) const {
    for (const auto& k : daftarKelas) {
        if (k->getId() == idKelas) return k.get();
    }
    return nullptr;
}

Ruangan* SistemAkademik::cariRuangan(const string& kode) const {
    for (const auto& r : daftarRuangan) {
        if (r->getKode() == kode) return r.get();
    }
    return nullptr;
}

MataKuliah* SistemAkademik::cariMataKuliah(const string& kode) const {
    for (const auto& m : daftarMataKuliah) {
        if (m->getKode() == kode) return m.get();
    }
    return nullptr;
}

// Kurikulum prodi tersebut dengan tahun TERBARU (mahasiswa baru memakai ini).
Kurikulum* SistemAkademik::getKurikulum(const string& prodi) const {
    Kurikulum* terbaru = nullptr;
    for (const auto& k : daftarKurikulum) {
        if (k->getProdi() != prodi) continue;
        if (!terbaru || k->getTahun() > terbaru->getTahun()) terbaru = k.get();
    }
    return terbaru;
}

MataKuliah* SistemAkademik::tambahMataKuliah(unique_ptr<MataKuliah> mk) {
    if (!mk || cariMataKuliah(mk->getKode())) return nullptr;
    daftarMataKuliah.push_back(std::move(mk));
    return daftarMataKuliah.back().get();
}

User* SistemAkademik::tambahUser(unique_ptr<User> user) {
    if (!user || cariUser(user->getNomorInduk())) return nullptr;
    daftarUser.push_back(std::move(user));
    return daftarUser.back().get();
}

Kelas* SistemAkademik::tambahKelas(unique_ptr<Kelas> kelas) {
    if (!kelas || cariKelas(kelas->getId())) return nullptr;
    daftarKelas.push_back(std::move(kelas));
    return daftarKelas.back().get();
}

Ruangan* SistemAkademik::tambahRuangan(unique_ptr<Ruangan> ruangan) {
    if (!ruangan || cariRuangan(ruangan->getKode())) return nullptr;
    daftarRuangan.push_back(std::move(ruangan));
    return daftarRuangan.back().get();
}

// Kunci kurikulum = (prodi, tahun).
Kurikulum* SistemAkademik::tambahKurikulum(unique_ptr<Kurikulum> kurikulum) {
    if (!kurikulum) return nullptr;
    for (const auto& k : daftarKurikulum) {
        if (k->getProdi() == kurikulum->getProdi() && k->getTahun() == kurikulum->getTahun()) return nullptr;
    }
    daftarKurikulum.push_back(std::move(kurikulum));
    return daftarKurikulum.back().get();
}

const vector<unique_ptr<User>>& SistemAkademik::getDaftarUser() const { return daftarUser; }
const vector<unique_ptr<Kelas>>& SistemAkademik::getDaftarKelas() const { return daftarKelas; }
const vector<unique_ptr<Ruangan>>& SistemAkademik::getDaftarRuangan() const { return daftarRuangan; }
const vector<unique_ptr<Kurikulum>>& SistemAkademik::getDaftarKurikulum() const { return daftarKurikulum; }
const vector<unique_ptr<MataKuliah>>& SistemAkademik::getDaftarMataKuliah() const { return daftarMataKuliah; }
bool SistemAkademik::muatData() { return penyimpanan && penyimpanan->muat(*this); }
bool SistemAkademik::simpanData() const { return penyimpanan && penyimpanan->simpan(*this); }
