# UnivLink
Universities Academic Management App
Anggota Kelompok
- Jundi Erlangga Luhur Prawira (25/561570/TK/63456)
- Yuma Binar Aryaputra (25/560457/TK/63303)

## 1. Arsitektur Aplikasi

Aplikasi ini menggunakan pola arsitektur berbasis objek (**Object-Oriented Programming / OOP**) di mana titik masuk program (*entry point*) dan manajer utamanya adalah class `SistemAkademik`. 

* **State Management**: Sistem ini menyimpan seluruh *state* (seperti pengguna yang sedang aktif) dan bertanggung jawab atas pengelolaan data dalam memori.
* **Enkapsulasi Penyimpanan**: Penyimpanan data dienkapsulasi menggunakan konsep Interface (`IPenyimpanan`). Dengan pendekatan ini, aplikasi untuk sementara dapat menyimpan data menggunakan file teks/CSV (`PenyimpananFile`), dan di masa mendatang dapat diganti ke penyimpanan Cloud atau Database tanpa perlu mengubah logika inti akademik.

---

## 2. Cara Kerja 4 Pilar OOP dalam Sistem

Aplikasi ini secara ketat menerapkan 4 pilar utama Object-Oriented Programming:

### A. Encapsulation (Enkapsulasi)
Membatasi hak akses data untuk menjaga integritas data:
* **Private (`-`)**: Atribut krusial seperti kata sandi (`passwordHash` pada `User`), IPK (`ipk` pada `Mahasiswa`), `listNilai`, dan kuota kelas disembunyikan. Data ini tidak dapat diubah langsung dari luar, melainkan harus melalui *method* yang memiliki validasi ketat.
* **Protected (`#`)**: Data yang dibutuhkan oleh class turunan, tetapi ditutup untuk umum. Contohnya: identitas `User` (`nomorInduk`, `nama`, `email`) yang perlu diakses oleh `Mahasiswa` atau `Dosen` untuk mencetak dashboard. Demikian juga `bobotKomponen` pada `MataKuliah`.
* **Public (`+`)**: Method antarmuka resmi (seperti `ambilKelas()`, `inputNilai()`) yang dapat dipanggil oleh komponen lain.

### B. Abstraction (Abstraksi)
Menyembunyikan kerumitan di balik interface:
* `User` dan `MataKuliah` adalah *abstract class* yang tidak dapat diinstansiasi langsung menjadi objek.
* Kompleksitas perhitungan seperti kalkulasi IPK disembunyikan di dalam class `Transkrip`.

### C. Inheritance (Pewarisan)
* Class `Mahasiswa`, `Dosen`, dan `Admin` mewarisi atribut identitas dan perilaku umum dari class `User`.
* Class `MataKuliahTeori` dan `MataKuliahPraktikum` mewarisi properti dari class `MataKuliah`.

### D. Polymorphism (Polimorfisme)
* **Dinamis (Overriding)**: Pemanggilan `tampilkanDashboard()` atau `getPeran()` pada pointer `User*` akan otomatis menyesuaikan dengan bentuk objek aslinya (`Mahasiswa`/`Dosen`/`Admin`). Hal serupa berlaku pada method `hitungNilaiAkhir()` pada class `MataKuliah`.
* **Statis (Overloading)**: Method `cariMataKuliah()` pada class `Kurikulum` memiliki dua versi berdasarkan tipe data parameternya; mencari berdasarkan `string` (kode mata kuliah) atau berdasarkan `integer` (semester).

---

## 3. Detail Class, Atribut, dan Method

Sistem dibagi menjadi beberapa kelompok (*Package*) agar modular dan mudah dirawat:

### A. Package Sistem
* **`SistemAkademik`**: Pemilik data (*owner*) dan pengatur alur utama.
  * *Fungsi*: Menyediakan fitur `login()`, `logout()`, serta pencarian user dan kelas. Class ini memiliki akses ke seluruh entitas objek di dalam sistem.
* **`IPenyimpanan` & `PenyimpananFile`**: Interface penyimpanan data.
  * Memiliki fungsi *pure virtual* `muat()` dan `simpan()` yang diimplementasikan oleh class `PenyimpananFile`.

### B. Package User
* **`User` (Abstract Class)**: Kerangka dasar pengguna.
  * *Atribut*: Menyimpan identitas (`NIM`/`NIP`, `nama`, `email`, `prodi`) dengan hak akses `protected`, dan `passwordHash` dengan hak akses `private`.
  * *Method*: `verifikasiPassword()`, `gantiPassword()`, serta deklarasi virtual `tampilkanDashboard()`.
* **`Mahasiswa`**: Bertindak sebagai aktor untuk fitur KRS dan akademik.
  * *Method Utama*: `buatKRS()`, `ambilKelas()`, `hitungMaxSKS()` (menghitung batas dinamis 15/18/21/24 SKS dari IPS), `lihatTranskrip()`, dan `submitTugas()`.
  * *Fungsi Khusus*: `cekPrasyarat()` yang mendelegasikan pengecekan kelulusan mata kuliah prasyarat kepada class `Transkrip`.
* **`Dosen`**: Aktor pengelola kegiatan belajar mengajar.
  * *Method Utama*: `buatPengumuman()`, `buatTugas()`, `beriSkorTugas()`, `inputNilai()`, `updateKehadiran()`, serta `finalisasiNilai()` untuk mengunci nilai di akhir semester.
* **`Admin`**: Aktor pengelola sistem (*Back-office*).
  * *Method Utama*: `buatKelas()`, `tambahRuangan()`, dan `assignRuangan()` ke kelas.

### C. Package Kurikulum & Mata Kuliah
* **`Kurikulum`**: Menyimpan peta semester (integrasi map semester dengan himpunan `MataKuliah`).
  * *Method Khusus*: `validasiGrafPrasyarat()` untuk mengecek apakah ada prasyarat berulang/siklis menggunakan algoritma Depth-First Search (DFS).
* **`MataKuliah` (Abstract Class)**: Menyimpan `kodeMK`, `SKS`, dan himpunan prasyarat.
  * *Pure Virtual Method*: `getJenisRuangan()` dan `hitungNilaiAkhir()`.
  * **`MataKuliahTeori`**: Jenis ruangannya adalah `KELAS`. Komponen nilai umumnya terdiri dari UTS, UAS, dan Tugas.
  * **`MataKuliahPraktikum`**: Jenis ruangannya adalah `LAB`. Komponen nilainya berbeda (misal: Praktik, Laporan, Responsi).

### D. Package Kelas & Kegiatan
* **`Kelas`**: Representasi operasional suatu mata kuliah di periode tertentu.
  * *Atribut*: `kuota`, `idKelas`, serta relasi ke `Pengumuman`, `Tugas`, `Jadwal`, dan `Ruangan`.
  * *Method*: `daftarkan()`, `catatKehadiran()` (presensi per pertemuan), `hitungPersentaseKehadiran()`, dan `hitungDistribusiNilai()` (membuat statistik A–E).
* **`Ruangan` & `Jadwal`**: Mengelola ketersediaan kelas.
  * *Method*: `tersedia()` dan `bentrokDengan()` untuk mengecek bentrokan waktu atau kondisi melebihi kapasitas ruangan.
* **`KRS` (Kartu Rencana Studi)**: Pemegang *state* registrasi per semester bagi setiap mahasiswa.
  * *Atribut & Fungsi*: Status `DRAFT` atau `DISAHKAN`. Method `validasi()` mengecek prasyarat, kuota, SKS maksimum, dan bentrok jadwal sebelum kelas ditambahkan.
* **`Tugas` & `Pengumpulan`**: `Tugas` memiliki *deadline* dan *bobot*, sedangkan `Pengumpulan` menyimpan *file*, *timestamp*, dan *skor* mahasiswa.
* **`Pertemuan`**: Menyimpan materi dan catatan presensi (map `Nomor Induk` dengan status kehadiran boolean) per sesi tatap muka.

### E. Package Penilaian
* **`Nilai`**: Terikat pada satu mahasiswa dan satu kelas. Menyimpan nilai per komponen.
  * *Fungsi*: `finalisasi()` mengkalkulasi skor angka menjadi `NilaiHuruf` dan membekukan statusnya agar dapat dimasukkan ke transkrip.
* **`Transkrip`**: Memiliki rekam jejak `Nilai` final.
  * *Fungsi Utama*: Mengeksekusi perhitungan akademik inti seperti `hitungIPS()`, `hitungIPK()`, dan memvalidasi `sudahLulus()` untuk pengecekan prasyarat.
* **`KonversiNilai`**: Berisi aturan *static* untuk mengonversi angka desimal ($0 - 100$) menjadi huruf mutu ($A$ sampai $E$) dan nilai bobot ($4.00$ sampai $0.00$).

---

## 4. Relasi Antar Class (UML)

1. **Composition (`*--`)**: Objek anak yang masa hidupnya ditentukan oleh induknya.
   * `Mahasiswa` memuat `Transkrip`.
   * `Kelas` memuat `Jadwal`, `Tugas`, dan `Pertemuan`. *(Jika kelas dihapus, maka jadwal, tugas, dan pertemuan di dalamnya ikut terhapus).*
2. **Aggregation (`o--`)**: Kepemilikan tidak mutlak (*weak association*).
   * `SistemAkademik` menampung daftar `User`.
   * `Kurikulum` menampung daftar `MataKuliah`.
3. **Association (`-->`)**: Relasi keterhubungan antar komponen.
   * `Kelas` diampu oleh satu `Dosen`.
   * `Kelas` berlangsung di sebuah `Ruangan`.
   * Relasi Prasyarat dibentuk sebagai *self-association* di dalam class `MataKuliah`.

---

## 5. Algoritma & Cara Kerja Sistem (Alur Eksekusi)

```
[Mahasiswa Tambah MK] ──► [1. Cek Status KRS]
                               │
                               ▼
                       [2. Cek Prasyarat Transkrip]
                               │
                               ▼
                       [3. Cek Kuota Kelas]
                               │
                               ▼
                       [4. Cek Limit SKS Dinamis]
                               │
                               ▼
                       [5. Cek Bentrok Jadwal]
                               │
                               ▼
                      (Terdaftar di Kelas)
```

### A. Alur Validasi KRS (Pendaftaran Mata Kuliah)
Ketika Mahasiswa menambah mata kuliah di KRS, sistem menjalankan serangkaian validasi berurutan:
1. **Cek Status**: Memastikan KRS masih dalam masa pendaftaran (`DRAFT`).
2. **Cek Prasyarat**: Sistem mengonfirmasi ke class `Transkrip` apakah mahasiswa telah lulus (huruf mutu di atas $E$) pada mata kuliah prasyarat yang dibutuhkan.
3. **Cek Kuota**: Memastikan jumlah peserta kelas belum melebihi batas kapasitas ($\text{peserta} < \text{kapasitas}$).
4. **Cek SKS**: $\text{Total SKS saat ini} + \text{SKS MK baru} \le \text{SKS Maksimal Dinamis}$.
5. **Cek Jadwal**: Memastikan tidak ada tumpang tindih waktu (jam dan hari) dengan mata kuliah lain yang sudah ada di dalam KRS.
6. **Eksekusi**: Jika seluruh poin 1–5 terpenuhi, mahasiswa berhasil terdaftar pada kelas tersebut.

### B. Alur Penentuan Batas SKS Dinamis
Sistem mengambil data dari `Transkrip` untuk menghitung IP Semester (IPS) terakhir. Batas maksimum SKS ditarik berdasarkan ketentuan berikut:

$$
\text{Batas SKS} = 
\begin{cases} 
24 \text{ SKS}, & \text{jika } IPS \ge 3.00 \\ 
21 \text{ SKS}, & \text{jika } 2.50 \le IPS \le 2.99 \\ 
18 \text{ SKS}, & \text{jika } 2.00 \le IPS \le 2.49 \\ 
15 \text{ SKS}, & \text{jika } IPS < 2.00 
\end{cases}
$$

### C. Alur Input dan Finalisasi Nilai
1. **Input Nilai**: Selama semester berjalan, Dosen menginput skor per komponen (misal: Tugas, Kuis, UTS) via method `inputNilai()`.
2. **Penyimpanan Sementara**: Data masuk ke objek `Nilai` dan masih bersifat dapat diubah (*editable*).
3. **Eksekusi Finalisasi**: Di akhir semester, Dosen memanggil method `finalisasiNilai()`.
4. **Kalkulasi Polimorfik**: Function `hitungNilaiAkhir()` pada `MataKuliah` diaktifkan untuk menghitung rata-rata berbobot seluruh skor komponen.
5. **Konversi Mutu**: Angka mentah ($0 - 100$) diproses oleh `KonversiNilai` untuk menghasilkan Huruf Mutu ($A - E$) dan nilai bobot ($4.00 - 0.00$).
6. **Pembekuan Status**: Status nilai terkunci (`isFinal() == true`) dan dikirim ke `Transkrip` untuk pemrosesan IPS/IPK.
