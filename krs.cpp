#include "univlink.h"
#include <algorithm>

// ===== KRS =====
// Semua aturan pengambilan kelas dipusatkan di KRS::validasi()

KRS::KRS(Mahasiswa* pemilik, const string& periode)
    : pemilik(pemilik), periode(periode), status(StatusKRS::DRAFT) {}

const string& KRS::getPeriode() const { return periode; }
StatusKRS KRS::getStatus() const { return status; }
Mahasiswa* KRS::getPemilik() const { return pemilik; }
const vector<Kelas*>& KRS::getDaftarKelas() const { return daftarKelas; }

// memuat: true jika kelas sudah ada di KRS (dicari berdasarkan alamat objek Kelas)
bool KRS::memuat(const Kelas* kelas) const {
    return find(daftarKelas.begin(), daftarKelas.end(), kelas) != daftarKelas.end();
}

// hitungTotalSKS: menjumlahkan SKS mata kuliah dari semua kelas di KRS
int KRS::hitungTotalSKS() const {
    int total = 0;
    for (const Kelas* k : daftarKelas) {
        if (k && k->getMataKuliah()) total += k->getMataKuliah()->getSKS();
    }
    return total;
}

// validasi: memeriksa apakah `kelas` boleh ditambahkan ke KRS
// Mengembalikan true jika semua aturan lolos. Jika ada yang gagal, mengembalikan false dan mengisi
// `alasan` dengan aturan pertama yang dilanggar, pemeriksaan berhenti

bool KRS::validasi(const Kelas& kelas, string& alasan) const {
    const MataKuliah* mk = kelas.getMataKuliah();

    if (!pemilik || !mk) {
        alasan = "Data KRS atau kelas tidak lengkap";
        return false;
    }
    if (status == StatusKRS::DISAHKAN) {
        alasan = "KRS sudah disahkan dan tidak dapat diubah";
        return false;
    }
    if (kelas.getPeriode() != periode) {
        alasan = "Periode kelas (" + kelas.getPeriode() + ") berbeda dengan periode KRS (" + periode + ")";
        return false;
    }
    if (memuat(&kelas)) {
        alasan = "Kelas " + kelas.getId() + " sudah ada di KRS";
        return false;
    }

    const Kurikulum* kur = pemilik->getKurikulum();
    if (kur && kur->cariMataKuliah(mk->getKode()) != mk) {
        alasan = mk->getNama() + " tidak ada di kurikulum mahasiswa";
        return false;
    }

    for (const Kelas* k : daftarKelas) {
        if (k && k->getMataKuliah() == mk) {
            alasan = mk->getNama() + " sudah diambil di kelas " + k->getId();
            return false;
        }
    }
    if (pemilik->getTranskrip().sudahLulus(mk->getKode())) {
        alasan = mk->getNama() + " sudah lulus";
        return false;
    }
    if (!pemilik->cekPrasyarat(*mk)) {
        alasan = "Prasyarat " + mk->getNama() + " belum terpenuhi";
        return false;
    }
    if (!kelas.masihAdaKuota()) {
        alasan = "Kuota kelas " + kelas.getId() + " sudah penuh";
        return false;
    }

    const int maxSKS = pemilik->hitungMaxSKS();
    const int totalBaru = hitungTotalSKS() + mk->getSKS();
    if (totalBaru > maxSKS) {
        alasan = "Melebihi batas SKS (" + to_string(totalBaru) + " dari maksimum " + to_string(maxSKS) + ")";
        return false;
    }

    for (const Kelas* k : daftarKelas) {
        if (k && k->bentrokDengan(kelas)) {
            alasan = "Jadwal bentrok dengan " + k->getMataKuliah()->getNama() + " (" + k->getJadwal().toString() + ")";
            return false;
        }
    }

    alasan.clear();
    return true;
}

// tambahKelas: menambahkan kelas ke KRS jika lolos validasi()
bool KRS::tambahKelas(Kelas* kelas) {
    if (!kelas) return false;

    string alasan;
    if (!validasi(*kelas, alasan)) return false;
    if (!kelas->daftarkan(this)) return false;

    daftarKelas.push_back(kelas);
    return true;
}

// hapusKelas: membatalkan satu kelas dari KRS
// ditolak jika KRS sudah DISAHKAN atau kelas tidak ada di KRS 
bool KRS::hapusKelas(Kelas* kelas) {
    if (!kelas || status == StatusKRS::DISAHKAN) return false;
    if (!memuat(kelas)) return false;

    kelas->keluarkan(this);
    daftarKelas.erase(remove(daftarKelas.begin(), daftarKelas.end(), kelas), daftarKelas.end());
    return true;
}

// sahkan: mengunci KRS (DRAFT -> DISAHKAN)
// Ditolak jika sudah disahkan, KRS kosong, atau total SKS 
// melebihi batas (jika batas berubah setelah kelas ditambahkan)
bool KRS::sahkan() {
    if (status == StatusKRS::DISAHKAN) return false;
    if (daftarKelas.empty() || !pemilik) return false;
    if (hitungTotalSKS() > pemilik->hitungMaxSKS()) return false;

    status = StatusKRS::DISAHKAN;
    return true;
}

// pulihkan: dipakai saat memuat data dari file
// Memasukkan kelas tanpa validasi (data lama dianggap sudah pernah valid), 
// tapi tetap menolak null/duplikat dan tetap mendaftarkan KRS ke kelas
bool KRS::pulihkan(Kelas* kelas) {
    if (!kelas || memuat(kelas)) return false;
    if (!kelas->daftarkan(this)) return false;

    daftarKelas.push_back(kelas);
    return true;
}

// pulihkanStatus: dipakai saat memuat data (mengatur status langsung dari file)
void KRS::pulihkanStatus(StatusKRS s) { status = s; }
