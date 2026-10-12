//nunggu yuma
//KRS::tambahKelas  memanggil Kelas::daftarkan(this)  (cek kuota ada di Kelas)
//KRS::hapusKelas   memanggil Kelas::keluarkan(this)
//KRS::getPemilik   dipakai Kelas::getPeserta / memilikiPeserta

#include "univlink.h"

#include <algorithm>

KRS::KRS(Mahasiswa* pemilik, const string& periode)
    : pemilik(pemilik), periode(periode), status(StatusKRS::DRAFT) {}

Mahasiswa* KRS::getPemilik() const { return pemilik; }

bool KRS::tambahKelas(Kelas* kelas) {
    if (!kelas || !kelas->daftarkan(this)) return false;   // KRS adalah friend Kelas
    daftarKelas.push_back(kelas);
    return true;
}

bool KRS::hapusKelas(Kelas* kelas) {
    if (!kelas || !kelas->keluarkan(this)) return false;
    daftarKelas.erase(remove(daftarKelas.begin(), daftarKelas.end(), kelas), daftarKelas.end());
    return true;
}

Mahasiswa::Mahasiswa(string nomorInduk, string nama, string email, string prodi,
                     const string& password, int angkatan, int semesterBerjalan,
                     Kurikulum* kurikulum, bool passwordSudahHash)
    : User(move(nomorInduk), move(nama), move(email), move(prodi), password, passwordSudahHash),
      angkatan(angkatan), semesterBerjalan(semesterBerjalan), kurikulum(kurikulum) {}

string Mahasiswa::getPeran() const { return "Mahasiswa"; }
void Mahasiswa::tampilkanDashboard() const { cetakHeader(); }
void Mahasiswa::tampilkanMenu() const {}
