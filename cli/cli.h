//dipisah karena ada rencana mau makai GUI kalau sempat
#ifndef CLI_H
#define CLI_H

#include "akademik/univlink.h"
#include <iostream>
#include <stdexcept>

// Dilempar bila cin habis (Ctrl+D / input dialihkan lalu selesai) agar loop menu tidak berputar selamanya. main() menangkapnya, menyimpan data, lalu keluar rapi.
struct InputBerakhir : std::runtime_error {
    InputBerakhir() : std::runtime_error("input berakhir") {}
};

namespace cli {

// ---- tampilan ----
void garis(char c = '-', int n = 60);
void judul(const string& teks);
string pangkas(const string& teks, size_t maks);          // potong + "..." bila kepanjangan
string ringkasDistribusi(const Kelas& kelas);             // "A:2 A-:1 B:3 (n=6)" atau "-"
string jenisKeString(const MataKuliah& mk);               // "Teori" / "Praktikum"

// ---- input (semua mengulang sampai valid; throw InputBerakhir bila cin habis) ----
string bacaBaris(const string& prompt);                              // boleh kosong
string bacaTidakKosong(const string& prompt);
int bacaInt(const string& prompt, int minimal, int maksimal);        // batas inklusif
double bacaDouble(const string& prompt, double minimal, double maksimal);
bool konfirmasi(const string& prompt);                               // y/n
string bacaPassword(const string& prompt);                           // tanpa echo bila terminal
void tunggu();                                                       // "Tekan Enter..."
// format "dd-mm-yyyy HH:MM"; kosong = batal (mengembalikan false)
bool bacaWaktu(const string& prompt, time_t& hasil);
// format "HH:MM" -> HHMM (mis. 07:30 -> 730)
int bacaJam(const string& prompt);
// memilih satu dari daftar bernomor 1..n; 0 = batal -> mengembalikan -1
int pilihNomor(const string& prompt, int jumlah);

// periode format "TTTT/TTTT-Gasal" atau "TTTT/TTTT-Genap" (tahun kedua = tahun pertama + 1)
bool periodeSah(const string& periode);
// meminta periode sampai sah; `bawaan` dipakai bila pengguna hanya menekan Enter (boleh kosong)
string bacaPeriode(const string& prompt, const string& bawaan);
// ganti kata sandi pengguna yang sedang login (minta sandi lama, baru, konfirmasi)
void layarGantiPassword(User& pengguna);
// daftar mahasiswa baru (dipakai layar login dan menu Admin); true bila berhasil dibuat
bool registrasiMahasiswa(SistemAkademik& sistem);
// tabel kelas bernomor (dipakai beberapa menu); mengembalikan jumlah baris
void cetakDaftarKelas(const vector<Kelas*>& daftar, bool tampilDosen);

// ---- bantu domain untuk antarmuka ----
// periode terbaru (urutan abjad terbesar) di antara semua kelas; "" bila belum ada kelas
string periodeTerbaru(const SistemAkademik& sistem);
// mengubah teks bebas ke huruf kecil tanpa spasi tepi (untuk nama komponen)
string normalisasi(const string& teks);

}  // namespace cli

// ---- menu per peran dan alur utama ----
void menuMahasiswa(SistemAkademik& sistem, Mahasiswa& mhs);
void menuDosen(SistemAkademik& sistem, Dosen& dosen);
void menuAdmin(SistemAkademik& sistem, Admin& admin);
void jalankanAplikasi(SistemAkademik& sistem);      // layar login/daftar -> menu peran

// data contoh (dipakai bila file data belum ada)
void isiDataAwal(SistemAkademik& sistem);

#endif
