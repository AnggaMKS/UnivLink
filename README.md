# UnivLink

**UnivLink** adalah aplikasi manajemen akademik universitas berbasis C++17 dan
Object-Oriented Programming (OOP). Aplikasi ini menyediakan alur akademik untuk
mahasiswa, dosen, dan admin: pengelolaan kurikulum, kelas, KRS, jadwal,
presensi, tugas, nilai, transkrip, serta penyimpanan data ke file.

Anggota kelompok:

- Jundi Erlangga Luhur Prawira (25/561570/TK/63456)
- Yuma Binar Aryaputra (25/560457/TK/63303)

## 1. Arsitektur UnivLink

UnivLink menggunakan arsitektur berlapis sederhana:

```text
CLI (input/output dan menu)
        |
        v
Application root: SistemAkademik
        |
        +--> Domain akademik
        |    User, Mahasiswa, Dosen, Admin
        |    Kurikulum, MataKuliah, Kelas, KRS
        |    Jadwal, Ruangan, Tugas, Nilai, Transkrip, ...
        |
        +--> Persistence abstraction: IPenyimpanan
                     |
                     v
             PenyimpananFile
```

### Domain layer

Deklarasi class domain berada di `akademik/univlink.h`, sedangkan implementasinya
dipisahkan ke beberapa file `.cpp`. Domain layer menangani aturan akademik,
misalnya validasi prasyarat, batas SKS, bentrok jadwal, kuota kelas, bobot
nilai, dan status KRS.

### Application root

`SistemAkademik` adalah aggregate root aplikasi. Class ini memiliki koleksi
user, mata kuliah, kelas, ruangan, dan kurikulum. Objek utama dimiliki melalui
`unique_ptr`, sedangkan hubungan antarobjek seperti kelas-dosen,
kelas-mata-kuliah, dan mahasiswa-kurikulum menggunakan pointer non-owning.
Class ini juga menangani login, logout, pencarian objek, pencegahan duplikasi,
serta delegasi penyimpanan.

### CLI layer

`cli.h` dan file `cli_*.cpp` menangani interaksi terminal: membaca input,
memvalidasi input, menampilkan menu, dan meneruskan aksi ke object domain.
Karena CLI dipisahkan dari domain, antarmuka lain seperti GUI dapat ditambahkan
tanpa memindahkan aturan bisnis ke kode menu.

### Persistence layer

`IPenyimpanan` adalah interface abstraksi penyimpanan. Implementasinya,
`PenyimpananFile`, menyimpan data ke `data/univlink.dat`. Penulisan dilakukan
melalui file sementara dan backup `data/univlink.bak` agar data lama tidak
langsung tertimpa ketika terjadi kegagalan.

## 2. Class dan Representasinya

### User dan role

- **`User`**: abstract base class untuk identitas pengguna. Menyimpan nomor
  induk, nama, email, prodi, password hash, dan operasi umum autentikasi.
- **`Mahasiswa`**: user mahasiswa. Memiliki KRS, transkrip, kurikulum,
  pendaftaran/batal kelas, validasi prasyarat, batas SKS, detail kelas, dan
  pengumpulan tugas.
- **`Dosen`**: user pengajar. Mengelola kelas yang diampu, pengumuman, tugas,
  skor tugas, komponen nilai, presensi, dan finalisasi nilai.
- **`Admin`**: user pengelola sistem. Mengelola ruangan, membuat kelas,
  menempatkan kelas ke ruangan dan jadwal, serta memvalidasi penempatan.

### Kurikulum dan mata kuliah

- **`Kurikulum`**: merepresentasikan kurikulum suatu prodi dan tahun. Class ini
  memetakan mata kuliah ke semester, menyediakan pencarian mata kuliah, dan
  memvalidasi graf prasyarat.
- **`MataKuliah`**: abstract class untuk mata kuliah, dengan kode, nama, SKS,
  semester, bobot komponen nilai, dan daftar prasyarat.
- **`MataKuliahTeori`**: mata kuliah yang membutuhkan ruangan bertipe `KELAS`
  dan menggunakan perhitungan nilai teori.
- **`MataKuliahPraktikum`**: mata kuliah yang membutuhkan ruangan bertipe
  `LAB` dan menggunakan perhitungan nilai praktikum.

### Kelas dan kegiatan akademik

- **`Kelas`**: penawaran mata kuliah pada periode tertentu. Menghubungkan mata
  kuliah, dosen, ruangan, jadwal, peserta KRS, tugas, pertemuan, dan
  pengumuman.
- **`KRS`**: rencana studi mahasiswa untuk satu periode. Menyimpan status
  `DRAFT` atau `DISAHKAN`, daftar kelas, total SKS, dan aturan validasi.
- **`Jadwal`**: hari, waktu mulai, dan waktu selesai; memvalidasi waktu dan
  mendeteksi bentrok.
- **`Ruangan`**: kode, gedung, kapasitas, jenis ruangan, dan jadwal kelas yang
  menggunakan ruangan.
- **`Pertemuan`**: satu sesi perkuliahan, termasuk nomor pertemuan, tanggal,
  materi, dan catatan kehadiran.
- **`Tugas`**: metadata tugas, deadline, bobot, dan daftar pengumpulan.
- **`Pengumpulan`**: file yang dikumpulkan mahasiswa, waktu pengumpulan, skor,
  dan status sudah dinilai.
- **`Pengumuman`**: pengumuman dosen yang dapat diedit dan dipublikasikan.

### Penilaian

- **`Nilai`**: nilai seorang mahasiswa pada satu kelas. Menyimpan komponen
  nilai, menghitung nilai akhir, mengonversi nilai huruf, dan mengunci nilai
  setelah finalisasi.
- **`Transkrip`**: kumpulan nilai final mahasiswa untuk menghitung IPS, IPK,
  mata kuliah lulus, dan total SKS lulus.
- **`KonversiNilai`**: utility statis untuk mengubah skor angka menjadi nilai
  huruf, bobot huruf, teks nilai, dan status lulus.

### Sistem dan penyimpanan

- **`IPenyimpanan`**: interface dengan operasi `muat()` dan `simpan()`.
- **`PenyimpananFile`**: implementasi `IPenyimpanan` berbasis file teks.
- **`SistemAkademik`**: pusat agregasi, lookup, autentikasi, dan persistence.

## 3. Empat Pilar OOP pada UnivLink

### A. Encapsulation (Enkapsulasi)

Data internal class dilindungi dan hanya dimodifikasi melalui method dengan
validasi:

- `User::passwordHash` bersifat private. Password diproses melalui
  `verifikasiPassword()` dan `gantiPassword()`, bukan diubah langsung.
- `KRS::status` dan daftar kelas diubah melalui `tambahKelas()`,
  `hapusKelas()`, `validasi()`, dan `sahkan()`.
- `Nilai` menyembunyikan komponen nilai, skor akhir, nilai huruf, bobot, dan
  status finalisasi. Perubahan dilakukan melalui `updateKomponen()` dan
  `finalisasi()`.
- `Kelas` memiliki koleksi tugas, pertemuan, dan pengumuman sebagai data
  internal. Akses dilakukan melalui method seperti `tambahTugas()`,
  `catatKehadiran()`, dan `getPengumumanAktif()`.
- `Transkrip` menyimpan `listNilai` secara private dan menyediakan operasi
  `hitungIPS()`, `hitungIPK()`, `sudahLulus()`, serta `totalSKSLulus()`.
- `SistemAkademik` mengontrol kepemilikan objek melalui `unique_ptr` dan
  mencegah duplikasi melalui `tambahUser()`, `tambahKelas()`,
  `tambahRuangan()`, dan method penambahan lainnya.
- Validasi domain menjaga konsistensi data: skor harus berada pada rentang
  yang benar, jadwal harus valid, ruangan harus cocok dengan jenis mata kuliah,
  kelas tidak boleh melebihi kuota, dan KRS harus memenuhi prasyarat serta
  batas SKS.

### B. Abstraction (Abstraksi)

Detail implementasi disembunyikan di balik interface dan operasi tingkat
tinggi:

- `User` adalah abstract class dengan operasi role, dashboard, dan menu yang
  didefinisikan secara virtual.
- `MataKuliah` adalah abstract class dengan operasi virtual
  `getJenisRuangan()` dan `hitungNilaiAkhir()`.
- `IPenyimpanan` menyembunyikan teknologi penyimpanan dari
  `SistemAkademik`; aplikasi hanya menggunakan `muat()` dan `simpan()`.
- `PenyimpananFile` menyembunyikan format record, escaping karakter, file
  sementara, dan backup.
- `Transkrip` menyembunyikan algoritma perhitungan IPS, IPK, dan status lulus.
- `KonversiNilai` menyembunyikan tabel konversi skor angka ke nilai huruf dan
  bobot.
- CLI memisahkan detail input/output terminal dari aturan bisnis domain.

### C. Inheritance (Pewarisan)

Hubungan pewarisan yang digunakan:

- `Mahasiswa`, `Dosen`, dan `Admin` mewarisi `User`.
- `MataKuliahTeori` dan `MataKuliahPraktikum` mewarisi `MataKuliah`.
- `PenyimpananFile` mewarisi `IPenyimpanan`.

Pewarisan memungkinkan setiap subclass menggunakan data dan perilaku umum
class induknya, lalu menambahkan perilaku khusus sesuai perannya.

### D. Polymorphism (Polimorfisme)

#### Dynamic polymorphism

- Pointer `User*` dapat menunjuk ke `Mahasiswa`, `Dosen`, atau `Admin`.
  Pemanggilan `getPeran()`, `tampilkanDashboard()`, dan `tampilkanMenu()`
  menjalankan override sesuai tipe object sebenarnya.
- Pointer `MataKuliah*` dapat menunjuk ke `MataKuliahTeori` atau
  `MataKuliahPraktikum`. Method `getJenisRuangan()` dan
  `hitungNilaiAkhir()` menghasilkan perilaku berbeda.
- Pointer `IPenyimpanan*` dapat menunjuk ke `PenyimpananFile`; pemanggilan
  `muat()` dan `simpan()` menggunakan implementasi file.
- `cli_login.cpp` mengarahkan user aktif ke menu role yang sesuai setelah
  login.

#### Static polymorphism melalui overloading

- `Kurikulum::cariMataKuliah(const string& kode)` mencari berdasarkan kode.
- `Kurikulum::cariMataKuliah(int semester)` mencari seluruh mata kuliah pada
  semester tertentu.

## 4. Struktur Folder dan File

Semua perintah build dijalankan dari folder root repository.

### Root project

| File | Deskripsi singkat |
|---|---|
| `README.md` | Dokumentasi proyek. |
| `.gitignore` | File hasil build dan data runtime yang diabaikan Git. |

Struktur utama:

```text
app/       entry point dan seed data
cli/       antarmuka terminal dan menu
akademik/  class domain akademik
storage/   implementasi persistence
data/      data runtime
references/ dan uml/  aset dokumentasi
```

| `akademik/univlink.h` | Deklarasi seluruh enum, class domain, interface, dan `SistemAkademik`. |
| `app/main.cpp` | Entry point: membuat persistence, memuat data, melakukan seed bila perlu, menjalankan CLI, lalu menyimpan data. |
| `app/seed.cpp` | Data contoh awal: user, mata kuliah, kurikulum, ruangan, kelas, dan jadwal. |

### `akademik/`

| File | Deskripsi singkat |
|---|---|
| `akademik/univlink.h` | Deklarasi class domain, interface, dan `SistemAkademik`. |
| `akademik/user.cpp` | Password hashing, verifikasi password, ganti password, dan perilaku umum user. |
| `akademik/mahasiswa.cpp` | Implementasi constructor dan operasi inti `Mahasiswa`. |
| `akademik/dosen.cpp` | Pengumuman, tugas, nilai, presensi, dan finalisasi nilai dosen. |
| `akademik/admin.cpp` | Pengelolaan ruangan serta pembuatan dan penempatan kelas. |
| `akademik/sistem.cpp` | Kepemilikan object, lookup, login/logout, dan delegasi persistence. |
| `akademik/kurikulum.cpp` | Implementasi mata kuliah, bobot nilai, prasyarat, dan kurikulum. |
| `akademik/kelas.cpp` | Pendaftaran peserta, tugas, pertemuan, pengumuman, presensi, dan statistik. |
| `akademik/kelas_statistik.cpp` | Perhitungan statistik kelas yang dipisahkan dari operasi utama `Kelas`. |
| `akademik/krs.cpp` | Validasi dan pengelolaan KRS. |
| `akademik/jadwal.cpp` | Validasi waktu dan bentrok jadwal. |
| `akademik/ruangan.cpp` | Kapasitas, tipe, pemesanan, dan pelepasan ruangan. |
| `akademik/kegiatan.cpp` | Implementasi `Pertemuan`, `Pengumpulan`, `Tugas`, dan `Pengumuman`. |
| `akademik/nilai.cpp` | Komponen nilai, nilai akhir, finalisasi, dan status lulus. |
| `akademik/transkrip.cpp` | Perhitungan IPS, IPK, nilai terbaik, dan SKS lulus. |
| `akademik/konversi_nilai.cpp` | Konversi skor angka ke nilai huruf dan bobot. |

### `cli/`

| File | Deskripsi singkat |
|---|---|
| `cli/cli.h` | Deklarasi fungsi CLI, menu, dan helper input. |
| `cli/cli_util.cpp` | Parsing input, validasi, formatting, tabel, dan layar ganti password. |
| `cli/cli_login.cpp` | Layar login, registrasi mahasiswa, dispatch role, dan logout. |
| `cli/cli_mahasiswa_menu.cpp` | Menu dan alur interaksi mahasiswa. |
| `cli/cli_dosen_menu.cpp` | Menu dan alur interaksi dosen. |
| `cli/cli_admin_menu.cpp` | Menu dan alur interaksi admin. |
| `cli/mahasiswa_cli.cpp` | Dashboard, transkrip, detail kelas, dan tampilan mahasiswa. |

### `storage/`

| File | Deskripsi singkat |
|---|---|
| `storage/penyimpanan_file.cpp` | Serialisasi/deserialisasi data ke `data/univlink.dat`. |

### Folder lain

- `tests/` berisi `test_b_phase1.cpp` dan `test_domain.cpp` untuk pengujian
  domain, KRS, penilaian, seed, jadwal, dan persistence.
- `uml/` berisi diagram class PlantUML (`univlinkclass.puml`) dan hasil
  visualisasi Draw.io (`univlink.drawio.png`).
- `proj/` berisi salinan/snapshot sumber terpisah dan tidak digunakan oleh
  Makefile root.
- `data/` dibuat saat aplikasi menyimpan data. File utamanya adalah
  `univlink.dat` dan backup-nya `univlink.bak`.

## 5. Build Aplikasi dengan g++

### Prasyarat

- GNU g++ yang mendukung C++17.
- GNU Make bersifat opsional; perintah `g++` langsung dapat digunakan.


### Cara build langsung dengan g++

Linux/macOS/Git Bash:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 -I. -o univlink \
  app/main.cpp app/seed.cpp \
  cli/cli_util.cpp cli/cli_login.cpp cli/cli_admin_menu.cpp \
  cli/cli_dosen_menu.cpp cli/cli_mahasiswa_menu.cpp cli/mahasiswa_cli.cpp \
  akademik/admin.cpp akademik/dosen.cpp akademik/jadwal.cpp \
  akademik/kegiatan.cpp akademik/kelas.cpp akademik/kelas_statistik.cpp \
  akademik/konversi_nilai.cpp akademik/krs.cpp akademik/kurikulum.cpp \
  akademik/mahasiswa.cpp akademik/nilai.cpp akademik/ruangan.cpp \
  akademik/sistem.cpp akademik/transkrip.cpp akademik/user.cpp \
  storage/penyimpanan_file.cpp
```

Windows PowerShell:

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 -I. -o univlink.exe `
  app/main.cpp app/seed.cpp `
  cli/cli_util.cpp cli/cli_login.cpp cli/cli_admin_menu.cpp `
  cli/cli_dosen_menu.cpp cli/cli_mahasiswa_menu.cpp cli/mahasiswa_cli.cpp `
  akademik/admin.cpp akademik/dosen.cpp akademik/jadwal.cpp `
  akademik/kegiatan.cpp akademik/kelas.cpp akademik/kelas_statistik.cpp `
  akademik/konversi_nilai.cpp akademik/krs.cpp akademik/kurikulum.cpp `
  akademik/mahasiswa.cpp akademik/nilai.cpp akademik/ruangan.cpp `
  akademik/sistem.cpp akademik/transkrip.cpp akademik/user.cpp `
  storage/penyimpanan_file.cpp
```
## 6. Cara Menjalankan Aplikasi

Jalankan dari root repository agar folder relatif `data/` ditemukan:

```bash
./univlink
```

Windows PowerShell:

```powershell
.\univlink.exe
```

Saat pertama kali dijalankan tanpa data user, `seed.cpp` membuat data contoh
secara otomatis. Data berikut dapat digunakan untuk login:

| Role | Nomor induk | Password |
|---|---|---|
| Admin | `A001` | `admin123` |
| Dosen | `D001` | `dosen123` |
| Dosen | `D002` | `dosen123` |
| Mahasiswa | `M001` | `mhs12345` |
| Mahasiswa | `M002` | `mhs12345` |
| Mahasiswa | `M003` | `mhs12345` |

Profil demo mahasiswa:

- `M001` adalah mahasiswa semester 3 dengan IPK 3.26 dan IPS semester 2
  sebesar 3.02. Seluruh tugas contoh pada kelasnya sudah dikumpulkan dan
  dinilai.
- `M002` adalah mahasiswa semester 5 dengan IPK 2.50 dan IPS semester 4
  sebesar 2.60. Hanya separuh tugas contoh yang dikumpulkan; sisanya tetap
  berstatus belum dikumpulkan.
- `M003` adalah mahasiswa semester 1 tanpa nilai historis.

Ganti password setelah login pertama apabila aplikasi digunakan di luar
lingkungan demo.

## 7. Cara Menggunakan Aplikasi

### Layar awal

```text
1. Masuk
2. Daftar akun mahasiswa
0. Keluar
```

Login dibatasi sampai tiga percobaan gagal. Setelah login, aplikasi
menampilkan menu sesuai role.

### Alur mahasiswa

1. Pilih **Registrasi Mata Kuliah (KRS)**.
2. Buat atau pilih KRS untuk periode berjalan.
3. Pilih kelas yang tersedia. Sistem memeriksa status KRS, prasyarat, kuota,
   batas SKS, dan bentrok jadwal.
4. Lihat atau batalkan kelas selama KRS masih dapat diubah.
5. Sahkan KRS setelah daftar kelas selesai.
6. Gunakan **Detail Kelas & Portal Tugas** untuk melihat jadwal, presensi,
   tugas, dan nilai.
7. Kumpulkan atau perbarui file tugas sebelum deadline.
8. Gunakan **Lihat Transkrip Nilai** untuk melihat IPS, IPK, nilai final, dan
   SKS lulus.
9. Gunakan **Lihat Pengumuman** dan **Ganti Kata Sandi** sesuai kebutuhan.

### Alur dosen

1. Pilih kelas yang diampu dari dashboard.
2. Buat, edit, dan publikasikan pengumuman.
3. Buat tugas dengan judul, deskripsi, deadline, dan bobot.
4. Lihat pengumpulan mahasiswa dan beri skor.
5. Catat presensi setiap pertemuan.
6. Input nilai berdasarkan komponen penilaian.
7. Periksa distribusi nilai, lalu finalisasi nilai ketika sudah selesai.
8. Logout untuk kembali ke layar awal dan menyimpan perubahan.

Operasi dosen dibatasi pada kelas yang benar-benar diampu oleh dosen tersebut.

### Alur admin

1. Kelola pengguna dosen, mahasiswa, dan admin.
2. Kelola mata kuliah, bobot penilaian, prasyarat, dan kurikulum.
3. Tambahkan ruangan dengan kapasitas dan tipe `KELAS` atau `LAB`.
4. Buat kelas untuk mata kuliah dan dosen pada periode tertentu.
5. Tempatkan kelas ke ruangan dan jadwal yang valid.
6. Sistem menolak penempatan yang tidak cocok tipe ruangan, melebihi kapasitas,
   atau bentrok dengan kelas lain.
7. Gunakan dashboard untuk memeriksa peserta dan detail kelas.

## 8. Format Data dan Siklus Program

Alur utama `app/main.cpp` adalah:

1. Membuat `PenyimpananFile("data")` dan `SistemAkademik`.
2. Memuat data dari `data/univlink.dat`.
3. Jika data belum memiliki user, memanggil `isiDataAwal()`.
4. Menjalankan `jalankanAplikasi()`.
5. Menyimpan perubahan ketika aplikasi selesai atau user logout sesuai alur
   CLI.

`PenyimpananFile` menyimpan record teks dengan pemisah `|` dan escaping untuk
karakter khusus. Record mencakup mata kuliah, prasyarat, kurikulum, ruangan,
user, kelas, KRS, pertemuan, tugas, pengumpulan, pengumuman, dan nilai.
Jika data rusak atau formatnya tidak valid, proses muat mengembalikan gagal
dan aplikasi tidak menimpa file data lama.
