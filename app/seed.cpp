// Data contoh yang dibuat saat folder data masih kosong, supaya aplikasi langsung bisa dicoba.
//Akun bawaan:
//   Admin     
//   1. A001  / admin123
//   Dosen     
//   1. D001  / dosen123      
//   2. D002 / dosen123
//   Mahasiswa 
//   1. M001 / mhs12345 (semester 3, IPK 3.26)
//   2. M002 / mhs12345 (semester 5, IPK 2.50)
//   3. M003 / mhs12345 (semester 1)

#include "cli/cli.h"

void isiDataAwal(SistemAkademik& sistem) {
    // ---- mata kuliah ----
    vector<MataKuliah*> semuaMataKuliah;
    auto tambah = [&](unique_ptr<MataKuliah> mk) {
        MataKuliah* hasil = sistem.tambahMataKuliah(std::move(mk));
        if (hasil) semuaMataKuliah.push_back(hasil);
        return hasil;
    };

    // Semester 1
    MataKuliah* pemrogramanDasar = tambah(make_unique<MataKuliahTeori>("TKU211131", "Pemrograman Dasar", 3, 1));
    MataKuliah* matematikaDiskrit = tambah(make_unique<MataKuliahTeori>("TKU211102", "Matematika Diskrit", 3, 1));
    MataKuliah* teoriVektorMatriks = tambah(make_unique<MataKuliahTeori>("TKU211104", "Teori Vektor dan Matriks", 2, 1));
    MataKuliah* fisikaMekanika = tambah(make_unique<MataKuliahTeori>("TKU211121", "Fisika Mekanika", 2, 1));
    MataKuliah* manajemenIndustri = tambah(make_unique<MataKuliahTeori>("TKU211122", "Manajemen Industri", 2, 1));
    MataKuliah* kalkulusVariabelTunggal = tambah(make_unique<MataKuliahTeori>("TKU211123", "Kalkulus Variabel Tunggal", 3, 1));
    MataKuliah* fisikaFluida = tambah(make_unique<MataKuliahTeori>("TKU211124", "Fisika Fluida", 2, 1));

    // Semester 2
    MataKuliah* praktikumPemrogramanDasar =
        tambah(make_unique<MataKuliahPraktikum>("TKU211131P", "Praktikum Pemrograman Dasar", 1, 2));
    MataKuliah* konsepTeknik = tambah(make_unique<MataKuliahTeori>("TKU211132", "Konsep Teknik", 2, 2));
    MataKuliah* AlgoritmaStrukturData =
        tambah(make_unique<MataKuliahTeori>("TIF212243", "Algoritma dan Struktur Data", 3, 2));
    MataKuliah* fisikaMedanListrik = tambah(make_unique<MataKuliahTeori>("TIF212244", "Fisika Medan Listrik", 3, 2));
    MataKuliah* probabilitasVariabelAcak =
        tambah(make_unique<MataKuliahTeori>("TIF212245", "Probabilitas Variabel Acak", 2, 2));
    MataKuliah* kalkulusVariabelJamak =
        tambah(make_unique<MataKuliahTeori>("TIF212246", "Kalkulus Variabel Jamak", 3, 2));
    MataKuliah* analisisVariabelKompleks =
        tambah(make_unique<MataKuliahTeori>("TIF212247", "Analisis Variabel Kompleks", 3, 2));
    MataKuliah* aljabarLinear = tambah(make_unique<MataKuliahTeori>("TIF212248", "Aljabar Linear", 3, 2));

    // Semester 3
    MataKuliah* statistika = tambah(make_unique<MataKuliahTeori>("TIF212250", "Statistika", 2, 3));
    MataKuliah* metodeNumeris = tambah(make_unique<MataKuliahTeori>("TIF212251", "Metode Numeris", 3, 3));
    MataKuliah* isyaratSistem = tambah(make_unique<MataKuliahTeori>("TIF212252", "Isyarat dan Sistem", 3, 3));
    MataKuliah* komunikasiDataKomputer =
        tambah(make_unique<MataKuliahTeori>("TIF212253", "Komunikasi Data dan Komputer", 3, 3));
    MataKuliah* pemrogramanOrientasiObjek =
        tambah(make_unique<MataKuliahTeori>("TIF212254", "Pemrograman Orientasi Objek", 3, 3));
    MataKuliah* arsitekturKomputer = tambah(make_unique<MataKuliahTeori>("TIF212255", "Arsitektur Komputer", 3, 3));
    MataKuliah* praktikumSainsDasar =
        tambah(make_unique<MataKuliahPraktikum>("TIF212256", "Praktikum Sains Dasar", 1, 3));
    MataKuliah* persamaanDifferensial =
        tambah(make_unique<MataKuliahTeori>("TIF212257", "Persamaan Differensial", 3, 3));

    // Semester 4
    MataKuliah* medanElektromagnetik =
        tambah(make_unique<MataKuliahTeori>("TIF212258", "Medan Elektromagnetik", 3, 4));
    MataKuliah* basisData = tambah(make_unique<MataKuliahTeori>("TIF212259", "Basis Data", 3, 4));
    MataKuliah* sistemMikroprosesor =
        tambah(make_unique<MataKuliahTeori>("TIF212260", "Sistem Berbasis Mikroprosesor", 3, 4));
    MataKuliah* kecerdasanBuatan = tambah(make_unique<MataKuliahTeori>("TIF212261", "Kecerdasan Buatan", 3, 4));
    MataKuliah* teknikVisualisasiGrafis =
        tambah(make_unique<MataKuliahTeori>("TIF212262", "Teknik Visualisasi Grafis", 3, 4));
    MataKuliah* jaringanKomputer = tambah(make_unique<MataKuliahTeori>("TIF212263", "Jaringan Komputer", 3, 4));
    MataKuliah* teknikPemodelanSimulasi =
        tambah(make_unique<MataKuliahTeori>("TIF212264", "Teknik Pemodelan dan Simulasi", 2, 4));

    // Semester 5
    MataKuliah* sistemOperasi = tambah(make_unique<MataKuliahTeori>("TIF212265", "Sistem Operasi", 3, 5));
    MataKuliah* kerjaPraktik = tambah(make_unique<MataKuliahTeori>("TIF212266", "Kerja Praktik", 2, 5));
    MataKuliah* projekJunior = tambah(
        make_unique<MataKuliahTeori>("TIF212267", "Projek Junior Teknologi Informasi", 2, 5));
    MataKuliah* rekayasaPerangkatLunak =
        tambah(make_unique<MataKuliahTeori>("TIF212268", "Rekayasa Perangkat Lunak", 3, 5));
    MataKuliah* rekayasaData = tambah(make_unique<MataKuliahTeori>("TIF212269", "Rekayasa Data", 3, 5));
    MataKuliah* pengembanganAplikasiWeb =
        tambah(make_unique<MataKuliahTeori>("TIF212270", "Pengembangan Aplikasi Web", 3, 5));
    MataKuliah* komputasiAwan = tambah(make_unique<MataKuliahTeori>("TIF212271", "Komputasi Awan", 2, 5));
    MataKuliah* interaksiManusiaKomputer =
        tambah(make_unique<MataKuliahTeori>("TIF212272", "Interaksi Manusia dan Komputer", 3, 5));

    // Semester 6
    MataKuliah* projekSenior = tambah(
        make_unique<MataKuliahTeori>("TIF212273", "Proyek Senior Teknologi Informasi", 3, 6));
    MataKuliah* capstoneSatu = tambah(make_unique<MataKuliahTeori>("TIF212274", "Capstone 1", 2, 6));
    MataKuliah* keamananKomputer = tambah(make_unique<MataKuliahTeori>("TIF212275", "Keamanan Komputer", 3, 6));
    MataKuliah* integrasiAplikasiInformasi =
        tambah(make_unique<MataKuliahTeori>("TIF212276", "Integrasi Aplikasi dan Informasi", 2, 6));
    MataKuliah* kewarganegaraan = tambah(make_unique<MataKuliahTeori>("TIF212277", "Kewarganegaraan", 2, 6));
    MataKuliah* pancasila = tambah(make_unique<MataKuliahTeori>("TIF212278", "Pancasila", 2, 6));
    MataKuliah* bahasaIndonesia = tambah(make_unique<MataKuliahTeori>("TIF212279", "Bahasa Indonesia", 2, 6));
    MataKuliah* agama = tambah(make_unique<MataKuliahTeori>("TIF212280", "Agama", 2, 6));

    // Semester 7
    MataKuliah* capstoneDua = tambah(make_unique<MataKuliahTeori>("TIF212281", "Capstone 2", 2, 7));
    MataKuliah* kuliahKerjaNyata = tambah(make_unique<MataKuliahTeori>("TIF212282", "Kuliah Kerja Nyata", 4, 7));
    MataKuliah* komunikasiMasyarakat =
        tambah(make_unique<MataKuliahTeori>("TIF212283", "Komunikasi Masyarakat", 2, 7));
    MataKuliah* penerapanTeknologiTepatGuna =
        tambah(make_unique<MataKuliahTeori>("TIF212284", "Penerapan Teknologi Tepat Guna", 2, 7));
    MataKuliah* kewirausahaanTeknik =
        tambah(make_unique<MataKuliahTeori>("TIF212285", "Kewirausahaan Teknik", 2, 7));
    MataKuliah* gameDevelopment = tambah(make_unique<MataKuliahTeori>("TIF212286", "Game Development", 3, 7));
    MataKuliah* mobileDevelopment = tambah(make_unique<MataKuliahTeori>("TIF212287", "Mobile Development", 3, 7));
    MataKuliah* softwareArchitecture =
        tambah(make_unique<MataKuliahTeori>("TIF212288", "Software Architecture", 3, 7));
    MataKuliah* softwareTesting = tambah(make_unique<MataKuliahTeori>("TIF212289", "Software Testing", 3, 7));
    MataKuliah* digitalForensics = tambah(make_unique<MataKuliahTeori>("TIF212290", "Digital Forensics", 3, 7));
    MataKuliah* ethicalHacking = tambah(make_unique<MataKuliahTeori>("TIF212291", "Ethical Hacking", 3, 7));
    MataKuliah* dataSecurityIntegrity =
        tambah(make_unique<MataKuliahTeori>("TIF212292", "Data Security and Integrity", 3, 7));
    MataKuliah* mobileCommunicationSystem =
        tambah(make_unique<MataKuliahTeori>("TIF212293", "Mobile Communication System", 3, 7));
    MataKuliah* naturalLanguageProcessing =
        tambah(make_unique<MataKuliahTeori>("TIF212294", "Natural Language Processing", 3, 7));
    MataKuliah* bigDataAnalysis =
        tambah(make_unique<MataKuliahTeori>("TIF212295", "Big Data and Analysis", 3, 7));
    MataKuliah* decisionSupportSystems =
        tambah(make_unique<MataKuliahTeori>("TIF212296", "Decision Support Systems", 3, 7));
    MataKuliah* dataCompression = tambah(make_unique<MataKuliahTeori>("TIF212297", "Data Compression", 3, 7));

    // Semester 8
    MataKuliah* topikKhusus = tambah(
        make_unique<MataKuliahTeori>("TIF212298", "Topik Khusus Teknologi Informasi", 3, 8));
    MataKuliah* digitalTransformation =
        tambah(make_unique<MataKuliahTeori>("TIF212299", "Digital Transformation", 3, 8));
    MataKuliah* userExperienceDesign =
        tambah(make_unique<MataKuliahTeori>("TIF212300", "User Experience Design", 3, 8));
    MataKuliah* imageProcessingComputerVision =
        tambah(make_unique<MataKuliahTeori>("TIF212301", "Image Processing and Computer Vision", 3, 8));
    MataKuliah* auditTataKelolaRisiko =
        tambah(make_unique<MataKuliahTeori>("TIF212302", "Audit TI dan Tata Kelola Risiko", 3, 8));
    MataKuliah* sistemInternetOfThings =
        tambah(make_unique<MataKuliahTeori>("TIF212303", "Sistem Internet of Things", 3, 8));
    MataKuliah* tikDanMasyarakat =
        tambah(make_unique<MataKuliahTeori>("TIF212304", "TIK dan Masyarakat", 3, 8));
    MataKuliah* teoriInformasiEntropi =
        tambah(make_unique<MataKuliahTeori>("TIF212305", "Teori Informasi dan Entropi Shanon", 3, 8));
    
    // ---- prasyarat ----
    praktikumPemrogramanDasar->tambahPrasyarat(pemrogramanDasar);
    AlgoritmaStrukturData->tambahPrasyarat(pemrogramanDasar);
    AlgoritmaStrukturData->tambahPrasyarat(matematikaDiskrit);
    AlgoritmaStrukturData->tambahPrasyarat(kalkulusVariabelTunggal);
    aljabarLinear->tambahPrasyarat(teoriVektorMatriks);
    fisikaMedanListrik->tambahPrasyarat(fisikaMekanika);
    praktikumSainsDasar->tambahPrasyarat(fisikaMedanListrik);
    praktikumSainsDasar->tambahPrasyarat(aljabarLinear);
    isyaratSistem->tambahPrasyarat(analisisVariabelKompleks);
    persamaanDifferensial->tambahPrasyarat(analisisVariabelKompleks);
    persamaanDifferensial->tambahPrasyarat(aljabarLinear);
    metodeNumeris->tambahPrasyarat(AlgoritmaStrukturData);
    statistika->tambahPrasyarat(probabilitasVariabelAcak);
    statistika->tambahPrasyarat(matematikaDiskrit);
    pemrogramanOrientasiObjek->tambahPrasyarat(pemrogramanDasar);
    medanElektromagnetik->tambahPrasyarat(fisikaMedanListrik);
    basisData->tambahPrasyarat(komunikasiDataKomputer);
    teknikVisualisasiGrafis->tambahPrasyarat(aljabarLinear);
    teknikPemodelanSimulasi->tambahPrasyarat(persamaanDifferensial);
    kecerdasanBuatan->tambahPrasyarat(aljabarLinear);
    kecerdasanBuatan->tambahPrasyarat(statistika);
    sistemMikroprosesor->tambahPrasyarat(arsitekturKomputer);
    komputasiAwan->tambahPrasyarat(basisData);
    komputasiAwan->tambahPrasyarat(fisikaMedanListrik);
    rekayasaData->tambahPrasyarat(basisData);
    sistemOperasi->tambahPrasyarat(arsitekturKomputer);
    projekJunior->tambahPrasyarat(pemrogramanOrientasiObjek);
    projekJunior->tambahPrasyarat(basisData);
    integrasiAplikasiInformasi->tambahPrasyarat(rekayasaPerangkatLunak);
    projekSenior->tambahPrasyarat(projekJunior);
    projekSenior->tambahPrasyarat(kecerdasanBuatan);
    projekSenior->tambahPrasyarat(jaringanKomputer);
    capstoneDua->tambahPrasyarat(capstoneSatu);





    // ---- kurikulum ----
    Kurikulum* kur = sistem.tambahKurikulum(make_unique<Kurikulum>("Teknologi Informasi", 2021));
    for (MataKuliah* mk : semuaMataKuliah) kur->tambahMataKuliah(mk);

    // ---- pengguna ----
    auto* admin = dynamic_cast<Admin*>(sistem.tambahUser(
        make_unique<Admin>("A001", "Admin Teknologi Informasi", "admin@teknologiinformasi.ugm.ac.id",
                           "Teknologi Informasi", "admin123", &sistem)));
    auto* d1 = dynamic_cast<Dosen*>(sistem.tambahUser(
        make_unique<Dosen>("D001", "Dr. Budi Santoso", "budi@ugm.ac.id", "Teknologi Informasi", "dosen123")));
    auto* d2 = dynamic_cast<Dosen*>(sistem.tambahUser(
        make_unique<Dosen>("D002", "Dr. Siti Rahma", "siti@ugm.ac.id", "Teknologi Informasi", "dosen123")));
    auto* m001 = dynamic_cast<Mahasiswa*>(sistem.tambahUser(
        make_unique<Mahasiswa>("M001", "Andi Pratama", "andi@mail.ugm.ac.id", "Teknologi Informasi",
                               "mhs12345", 2021, 3, kur)));
    auto* m002 = dynamic_cast<Mahasiswa*>(sistem.tambahUser(
        make_unique<Mahasiswa>("M002", "Dewi Lestari", "dewi@mail.ugm.ac.id", "Teknologi Informasi",
                               "mhs12345", 2021, 5, kur)));
    auto* m003 = dynamic_cast<Mahasiswa*>(sistem.tambahUser(
        make_unique<Mahasiswa>("M003", "Fajar Nugraha", "fajar@mail.ugm.ac.id", "Teknologi Informasi",
                               "mhs12345", 2021, 1, kur)));
    if (!admin || !d1 || !d2) return;

    // ---- ruangan ----
    admin->tambahRuangan(make_unique<Ruangan>("R101", "Gedung Utara", 40, JenisRuangan::KELAS));
    admin->tambahRuangan(make_unique<Ruangan>("R102", "Gedung Utara", 40, JenisRuangan::KELAS));
    admin->tambahRuangan(make_unique<Ruangan>("LAB1", "Gedung Selatan", 30, JenisRuangan::LAB));
    for (int nomor = 103; nomor <= 110; ++nomor) {
        admin->tambahRuangan(make_unique<Ruangan>(
            "R" + to_string(nomor), "Gedung Utara", 40, JenisRuangan::KELAS));
    }
    admin->tambahRuangan(make_unique<Ruangan>("LAB2", "Gedung Selatan", 30, JenisRuangan::LAB));
    admin->tambahRuangan(make_unique<Ruangan>("LAB3", "Gedung Selatan", 30, JenisRuangan::LAB));

    vector<Ruangan*> ruangKelas;
    vector<Ruangan*> ruangLab;
    for (const auto& ruang : sistem.getDaftarRuangan()) {
        if (ruang->getJenis() == JenisRuangan::KELAS) ruangKelas.push_back(ruang.get());
        else ruangLab.push_back(ruang.get());
    }

    // ---- kelas periode berjalan + penempatan ----
    const string periode = "2021/2027-Gasal";
    struct Rencana { MataKuliah* mk; Dosen* dosen; int kuota; Ruangan* ruang; Hari hari; int mulai, selesai; };
    const Hari hari[] = {Hari::SENIN, Hari::SELASA, Hari::RABU, Hari::KAMIS, Hari::JUMAT};
    const int jamMulai[] = {700, 900, 1100, 1300, 1500, 1700, 1900, 2100};
    vector<Rencana> rencana;
    rencana.reserve(semuaMataKuliah.size());
    for (size_t i = 0; i < semuaMataKuliah.size(); ++i) {
        MataKuliah* mk = semuaMataKuliah[i];
        const size_t slot = i / 2;
        const bool praktikum = mk->getJenisRuangan() == JenisRuangan::LAB;
        vector<Ruangan*>& ruangTersedia = praktikum ? ruangLab : ruangKelas;
        Ruangan* ruang = ruangTersedia[i % ruangTersedia.size()];
        Dosen* dosen = (i % 2 == 0) ? d1 : d2;
        const int mulai = jamMulai[slot / 5];
        const int selesai = mulai + 130;
        rencana.push_back({mk, dosen, praktikum ? 25 : 35, ruang,
                           hari[slot % 5], mulai, selesai});
    }
    for (const Rencana& r : rencana) {
        Kelas* k = admin->buatKelas(*r.mk, *r.dosen, periode, r.kuota);
        if (k) admin->assignRuangan(*k, *r.ruang, Jadwal(r.hari, r.mulai, r.selesai));
    }

    // ---- profil mahasiswa contoh ----
    // Nilai historis dibuat pada kelas terpisah agar IPS semester terakhir dan
    // IPK dapat dicoba langsung dari dashboard tanpa mengubah KRS berjalan.
    auto buatKelasHistoris = [&](MataKuliah* mk, Dosen* dosen, const string& periodeHistoris,
                                 size_t urutan) -> Kelas* {
        if (!mk || !dosen) return nullptr;
        Kelas* kelas = admin->buatKelas(*mk, *dosen, periodeHistoris, 40);
        if (!kelas) return nullptr;
        vector<Ruangan*>& ruang = mk->getJenisRuangan() == JenisRuangan::LAB ? ruangLab : ruangKelas;
        if (ruang.empty()) return nullptr;
        const int mulai = 700 + static_cast<int>((urutan % 6) * 200);
        const Hari hariHistoris[] = {Hari::SENIN, Hari::SELASA, Hari::RABU,
                                     Hari::KAMIS, Hari::JUMAT};
        admin->assignRuangan(*kelas, *ruang[urutan % ruang.size()],
                             Jadwal(hariHistoris[urutan % 5], mulai, mulai + 130));
        return kelas;
    };

    auto skorUntukBobot = [](int bobotPerEmpat) {
        switch (bobotPerEmpat) {
            case 0: return 0.0;
            case 4: return 40.0;
            case 7: return 45.0;
            case 8: return 50.0;
            case 9: return 55.0;
            case 10: return 60.0;
            case 11: return 65.0;
            case 12: return 70.0;
            case 13: return 75.0;
            case 14: return 80.0;
            case 15: return 85.0;
            default: return 95.0;
        }
    };

    auto beriNilaiHistoris = [&](Mahasiswa* mahasiswa, Kelas* kelas, int bobotPerEmpat) {
        if (!mahasiswa || !kelas) return;
        Nilai* nilai = mahasiswa->getTranskrip().ambilAtauBuatNilai(kelas);
        if (!nilai) return;
        const double skor = skorUntukBobot(bobotPerEmpat);
        for (const auto& [komponen, bobot] : kelas->getMataKuliah()->getBobotKomponen()) {
            (void)bobot;
            nilai->updateKomponen(komponen, skor);
        }
        nilai->finalisasi();
    };

    auto tambahNilaiPeriode = [&](Mahasiswa* mahasiswa, const vector<MataKuliah*>& mataKuliah,
                                  const vector<int>& bobot, const string& periodeHistoris,
                                  size_t offset) {
        for (size_t i = 0; i < mataKuliah.size() && i < bobot.size(); ++i) {
            Kelas* kelas = buatKelasHistoris(mataKuliah[i], (i + offset) % 2 == 0 ? d1 : d2,
                                             periodeHistoris, offset + i);
            beriNilaiHistoris(mahasiswa, kelas, bobot[i]);
        }
    };

    // M001: 17 SKS semester pertama dan 16 SKS semester kedua.
    // Kombinasi bobot menghasilkan IPK 3.26 dan IPS semester kedua 3.02.
    tambahNilaiPeriode(m001,
        {pemrogramanDasar, matematikaDiskrit, teoriVektorMatriks, fisikaMekanika,
         manajemenIndustri, kalkulusVariabelTunggal, fisikaFluida},
        {7, 14, 15, 16, 16, 16, 16}, "2021/2022-Gasal", 0);
    tambahNilaiPeriode(m001,
        {praktikumPemrogramanDasar, konsepTeknik, AlgoritmaStrukturData, fisikaMedanListrik,
         probabilitasVariabelAcak, kalkulusVariabelJamak, analisisVariabelKompleks},
        {0, 4, 14, 16, 16, 16, 15}, "2021/2022-Genap", 8);

    // M002: 20 SKS nilai lama dengan rata-rata 2.40 dan 20 SKS semester
    // keempat dengan IPS 2.60, sehingga IPK kumulatifnya 2.50.
    tambahNilaiPeriode(m002,
        {pemrogramanDasar, matematikaDiskrit, teoriVektorMatriks, fisikaMekanika,
         manajemenIndustri, kalkulusVariabelTunggal, fisikaFluida,
         konsepTeknik},
        {10, 10, 8, 8, 10, 10, 10, 10}, "2021/2022-Gasal", 16);
    tambahNilaiPeriode(m002,
        {medanElektromagnetik, basisData, sistemMikroprosesor, kecerdasanBuatan,
         teknikVisualisasiGrafis, jaringanKomputer, teknikPemodelanSimulasi, sistemOperasi},
        {0, 0, 8, 16, 16, 16, 16, 16}, "2022/2023-Genap", 24);

    auto cariKelasBerjalan = [&](MataKuliah* mk) -> Kelas* {
        for (const auto& kelas : sistem.getDaftarKelas()) {
            if (kelas && kelas->getMataKuliah() == mk && kelas->getPeriode() == periode) {
                return kelas.get();
            }
        }
        return nullptr;
    };

    auto tambahTugasContoh = [&](Mahasiswa* mahasiswa, MataKuliah* mk, int jumlah,
                                 int jumlahDikumpulkan) {
        Kelas* kelas = cariKelasBerjalan(mk);
        if (!mahasiswa || !kelas || !mahasiswa->ambilKelas(*kelas)) return;
        for (int i = 0; i < jumlah; ++i) {
            Tugas* tugas = kelas->getDosen()->buatTugas(
                *kelas, "Tugas " + to_string(i + 1) + " - " + mk->getNama(),
                "Tugas contoh untuk demonstrasi portal mahasiswa.",
                time(nullptr) + 30 * 24 * 60 * 60, 50.0);
            if (!tugas) continue;
            if (i < jumlahDikumpulkan &&
                mahasiswa->submitTugas(*tugas, "berkas-contoh-" + to_string(i + 1) + ".pdf")) {
                kelas->getDosen()->beriSkorTugas(*tugas, *mahasiswa, 90.0);
            }
        }
    };

    // M001 memiliki tugas yang seluruhnya sudah dikumpulkan dan diberi nilai.
    tambahTugasContoh(m001, arsitekturKomputer, 2, 2);
    tambahTugasContoh(m001, komunikasiDataKomputer, 2, 2);
    // M002 hanya mengumpulkan separuh tugas; sisanya sengaja dibiarkan kosong.
    tambahTugasContoh(m002, pemrogramanOrientasiObjek, 4, 2);
    // M003 adalah mahasiswa baru semester pertama tanpa nilai historis.
    if (Kelas* kelas = cariKelasBerjalan(pemrogramanDasar)) m003->ambilKelas(*kelas);
}
