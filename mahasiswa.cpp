// Konstruktor Mahasiswa 
#include "univlink.h"

Mahasiswa::Mahasiswa(string nomorInduk, string nama, string email, string prodi,
                     const string& password, int angkatan, int semesterBerjalan,
                     Kurikulum* kurikulum, bool passwordSudahHash)
    : User(std::move(nomorInduk), std::move(nama), std::move(email), std::move(prodi),
           password, passwordSudahHash),
      angkatan(angkatan),
      semesterBerjalan(semesterBerjalan),
      kurikulum(kurikulum) {}
