#include "univlink.h"
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;
namespace {
const char* NAMA_BERKAS = "univlink.dat";
const int VERSI_FORMAT = 1;

// escape atau split
string esc(const string& s) {
    string r;
    r.reserve(s.size());
    for (char c : s) {
        switch (c) {
            case '\\': r += "\\\\"; break;
            case '|':  r += "\\p"; break;
            case ';':  r += "\\s"; break;
            case '=':  r += "\\e"; break;
            case '\n': r += "\\n"; break;
            case '\r': r += "\\r"; break;
            default:   r += c;
        }
    }
    return r;
}

string unesc(const string& s) {
    string r;
    r.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] != '\\' || i + 1 >= s.size()) { r += s[i]; continue; }
        switch (s[++i]) {
            case '\\': r += '\\'; break;
            case 'p':  r += '|'; break;
            case 's':  r += ';'; break;
            case 'e':  r += '='; break;
            case 'n':  r += '\n'; break;
            case 'r':  r += '\r'; break;
            default:   r += s[i];
        }
    }
    return r;
}

// Memecah menurut pemisah mentah; string kosong -> daftar kosong.
vector<string> pecah(const string& s, char sep) {
    vector<string> hasil;
    if (s.empty()) return hasil;
    string cur;
    for (char c : s) {
        if (c == sep) { hasil.push_back(cur); cur.clear(); }
        else cur += c;
    }
    hasil.push_back(cur);
    return hasil;
}

// Memecah baris rekaman: kolom kosong di ujung tetap dihitung ("a||" -> 3 kolom).
vector<string> pecahKolom(const string& s) {
    vector<string> hasil;
    string cur;
    for (char c : s) {
        if (c == '|') { hasil.push_back(cur); cur.clear(); }
        else cur += c;
    }
    hasil.push_back(cur);
    return hasil;
}

string angka(double d) {
    char buf[40];
    snprintf(buf, sizeof(buf), "%.10g", d);
    return buf;
}
string angka(long long v) { return to_string(v); }

bool keInt(const string& s, int& out) {
    if (s.empty()) return false;
    char* akhir = nullptr;
    long v = strtol(s.c_str(), &akhir, 10);
    if (*akhir != '\0') return false;
    out = static_cast<int>(v);
    return true;
}
bool keLL(const string& s, long long& out) {
    if (s.empty()) return false;
    char* akhir = nullptr;
    out = strtoll(s.c_str(), &akhir, 10);
    return *akhir == '\0';
}
bool keDouble(const string& s, double& out) {
    if (s.empty()) return false;
    char* akhir = nullptr;
    out = strtod(s.c_str(), &akhir);
    return *akhir == '\0';
}

// komposisi "k=v;k=v" dari map
string tulisMap(const map<string, double>& m) {
    string r;
    for (const auto& [k, v] : m) {
        if (!r.empty()) r += ';';
        r += esc(k) + "=" + angka(v);
    }
    return r;
}

// baca komposisi "k=v;k=v" -> map; false bila bentuknya salah
bool bacaMap(const string& s, map<string, double>& out) {
    out.clear();
    for (const string& pasangan : pecah(s, ';')) {
        const vector<string> kv = pecah(pasangan, '=');
        double v = 0;
        if (kv.size() != 2 || !keDouble(kv[1], v)) return false;
        out[unesc(kv[0])] = v;
    }
    return true;
}

void tulisBaris(ostream& out, const vector<string>& kolom) {
    for (size_t i = 0; i < kolom.size(); ++i) {
        if (i) out << '|';
        out << kolom[i];
    }
    out << '\n';
}

}

PenyimpananFile::PenyimpananFile(string folderData) : folderData(std::move(folderData)) {}

// SIMPAN
bool PenyimpananFile::simpan(const SistemAkademik& sistem) {
    error_code ec;
    const fs::path dir(folderData);
    fs::create_directories(dir, ec);
    if (ec) { cerr << "Gagal membuat folder data: " << ec.message() << '\n'; return false; }

    const fs::path berkas = dir / NAMA_BERKAS;
    const fs::path sementara = dir / (string(NAMA_BERKAS) + ".tmp");
    const fs::path cadangan = dir / "univlink.bak";

    {
        ofstream out(sementara, ios::trunc | ios::binary);
        if (!out) { cerr << "Gagal membuka berkas sementara\n"; return false; }

        tulisBaris(out, {"V", to_string(VERSI_FORMAT)});

        // mata kuliah + prasyarat
        for (const auto& mk : sistem.getDaftarMataKuliah()) {
            const bool praktikum = dynamic_cast<const MataKuliahPraktikum*>(mk.get()) != nullptr;
            tulisBaris(out, {"MK", praktikum ? "P" : "T", esc(mk->getKode()), esc(mk->getNama()),
                             to_string(mk->getSKS()), to_string(mk->getSemester()),
                             tulisMap(mk->getBobotKomponen())});
        }
        for (const auto& mk : sistem.getDaftarMataKuliah()) {
            for (const MataKuliah* pre : mk->getPrasyarat()) {
                tulisBaris(out, {"PRE", esc(mk->getKode()), esc(pre->getKode())});
            }
        }
        // kurikulum
        for (const auto& kur : sistem.getDaftarKurikulum()) {
            tulisBaris(out, {"KUR", esc(kur->getProdi()), to_string(kur->getTahun())});
            for (const MataKuliah* mk : kur->semuaMataKuliah()) {
                tulisBaris(out, {"KURMK", esc(kur->getProdi()), to_string(kur->getTahun()), esc(mk->getKode())});
            }
        }
        // ruangan
        for (const auto& r : sistem.getDaftarRuangan()) {
            tulisBaris(out, {"RU", esc(r->getKode()), esc(r->getGedung()), to_string(r->getKapasitas()),
                             r->getJenis() == JenisRuangan::LAB ? "LAB" : "KELAS"});
        }
        // pengguna
        for (const auto& u : sistem.getDaftarUser()) {
            vector<string> k = {"USR", "A", esc(u->getNomorInduk()), esc(u->getNama()), esc(u->getEmail()),
                                esc(u->getProdi()), esc(u->passwordHash), "", "", "", ""};
            if (const auto* m = dynamic_cast<const Mahasiswa*>(u.get())) {
                k[1] = "M";
                k[7] = to_string(m->getAngkatan());
                k[8] = to_string(m->getSemesterBerjalan());
                if (m->getKurikulum()) {
                    k[9] = esc(m->getKurikulum()->getProdi());
                    k[10] = to_string(m->getKurikulum()->getTahun());
                }
            } else if (dynamic_cast<const Dosen*>(u.get())) {
                k[1] = "D";
            }
            tulisBaris(out, k);
        }
        // kelas
        for (const auto& kl : sistem.getDaftarKelas()) {
            string ruang, hari, mulai, selesai;
            if (kl->getRuangan() && kl->punyaJadwal()) {
                ruang = esc(kl->getRuangan()->getKode());
                hari = to_string(static_cast<int>(kl->getJadwal().getHari()));
                mulai = to_string(kl->getJadwal().getJamMulai());
                selesai = to_string(kl->getJadwal().getJamSelesai());
            }
            tulisBaris(out, {"KEL", esc(kl->getId()), esc(kl->getMataKuliah()->getKode()),
                             esc(kl->getDosen()->getNomorInduk()), esc(kl->getPeriode()),
                             to_string(kl->getKuota()), to_string(kl->getTotalPertemuan()),
                             ruang, hari, mulai, selesai});
        }
        // KRS
        for (const auto& u : sistem.getDaftarUser()) {
            const auto* m = dynamic_cast<const Mahasiswa*>(u.get());
            if (!m) continue;
            for (const auto& krs : m->getDaftarKRS()) {
                string daftar;
                for (const Kelas* k : krs->getDaftarKelas()) {
                    if (!daftar.empty()) daftar += ';';
                    daftar += esc(k->getId());
                }
                tulisBaris(out, {"KRS", esc(m->getNomorInduk()), esc(krs->getPeriode()),
                                 krs->getStatus() == StatusKRS::DISAHKAN ? "1" : "0", daftar});
            }
        }
        // isi kelas: pertemuan, tugas, pengumpulan, pengumuman
        for (const auto& kl : sistem.getDaftarKelas()) {
            for (const Pertemuan* p : kl->getDaftarPertemuan()) {
                string hadir;
                for (const auto& [niu, h] : p->getKehadiran()) {
                    if (!hadir.empty()) hadir += ';';
                    hadir += esc(niu) + "=" + (h ? "1" : "0");
                }
                tulisBaris(out, {"PER", esc(kl->getId()), to_string(p->getNomor()),
                                 angka(static_cast<long long>(p->getTanggal())), esc(p->getMateri()), hadir});
            }
            for (const Tugas* t : kl->getDaftarTugas()) {
                tulisBaris(out, {"TGS", esc(kl->getId()), esc(t->getId()), esc(t->getJudul()), esc(t->getDeskripsi()),
                                 angka(static_cast<long long>(t->getDeadline())), angka(t->getBobot())});
                for (const Pengumpulan* pg : t->getDaftarPengumpulan()) {
                    if (!pg->getPengumpul()) continue;
                    tulisBaris(out, {"PGP", esc(kl->getId()), esc(t->getId()), esc(pg->getPengumpul()->getNomorInduk()),
                                     esc(pg->getFilePath()), angka(static_cast<long long>(pg->getWaktuKumpul())),
                                     angka(pg->getSkor()), pg->isSudahDinilai() ? "1" : "0"});
                }
            }
            for (const Pengumuman* pn : kl->getDaftarPengumuman()) {
                tulisBaris(out, {"PNG", esc(kl->getId()), esc(pn->getId()), esc(pn->getJudul()), esc(pn->getIsi()),
                                 angka(static_cast<long long>(pn->getTanggal())),
                                 pn->getStatus() == StatusPengumuman::TERBIT ? "1" : "0",
                                 pn->getPembuat() ? esc(pn->getPembuat()->getNomorInduk()) : ""});
            }
        }
        // nilai
        for (const auto& u : sistem.getDaftarUser()) {
            const auto* m = dynamic_cast<const Mahasiswa*>(u.get());
            if (!m) continue;
            for (const auto& n : m->getTranskrip().getListNilai()) {
                if (!n->getKelas()) continue;
                tulisBaris(out, {"NIL", esc(m->getNomorInduk()), esc(n->getKelas()->getId()),
                                 n->isFinal() ? "1" : "0", tulisMap(n->getKomponen())});
            }
        }
        out.flush();
        if (!out) { cerr << "Gagal menulis data (disk penuh?)\n"; out.close(); fs::remove(sementara, ec); return false; }
    }

    // cadangkan berkas lama, lalu ganti dengan yang baru
    if (fs::exists(berkas, ec)) {
        fs::copy_file(berkas, cadangan, fs::copy_options::overwrite_existing, ec);
    }
    fs::rename(sementara, berkas, ec);
    if (ec) { cerr << "Gagal mengganti berkas data: " << ec.message() << '\n'; return false; }
    return true;
}


// MUAT
bool PenyimpananFile::muat(SistemAkademik& sistem) {
    error_code ec;
    const fs::path berkas = fs::path(folderData) / NAMA_BERKAS;
    if (!fs::exists(berkas, ec)) return true; 

    ifstream in(berkas, ios::binary);
    if (!in) { cerr << "Tidak dapat membuka " << berkas.string() << '\n'; return false; }

    int nomorBaris = 0;
    string baris;
    auto gagal = [&](const string& alasan) {
        cerr << "Berkas data rusak pada baris " << nomorBaris << ": " << alasan << '\n';
        return false;
    };
    auto cariKur = [&](const string& prodi, int tahun) -> Kurikulum* {
        for (const auto& k : sistem.getDaftarKurikulum()) {
            if (k->getProdi() == prodi && k->getTahun() == tahun) return k.get();
        }
        return nullptr;
    };
    auto cariMhs = [&](const string& niu) { return dynamic_cast<Mahasiswa*>(sistem.cariUser(niu)); };

    bool adaVersi = false;
    while (getline(in, baris)) {
        ++nomorBaris;
        if (!baris.empty() && baris.back() == '\r') baris.pop_back();
        if (baris.empty()) continue;

        const vector<string> k = pecahKolom(baris);
        const string& tag = k[0];
        auto butuh = [&](size_t n) { return k.size() >= n; };

        if (tag == "V") {
            int v = 0;
            if (!butuh(2) || !keInt(k[1], v) || v != VERSI_FORMAT) return gagal("versi format tidak dikenal");
            adaVersi = true;
            continue;
        }
        if (!adaVersi) return gagal("baris versi (V) tidak ditemukan di awal berkas");

        if (tag == "MK") {
            int sks = 0, smt = 0;
            map<string, double> bobot;
            if (!butuh(7) || !keInt(k[4], sks) || !keInt(k[5], smt) || !bacaMap(k[6], bobot)) return gagal("rekaman MK salah");
            unique_ptr<MataKuliah> mk;
            if (k[1] == "P") mk = make_unique<MataKuliahPraktikum>(unesc(k[2]), unesc(k[3]), sks, smt);
            else             mk = make_unique<MataKuliahTeori>(unesc(k[2]), unesc(k[3]), sks, smt);
            if (!bobot.empty() && !mk->setBobotKomponen(bobot)) return gagal("bobot komponen MK harus berjumlah 100");
            if (!sistem.tambahMataKuliah(std::move(mk))) return gagal("kode mata kuliah kembar");
        } else if (tag == "PRE") {
            if (!butuh(3)) return gagal("rekaman PRE salah");
            MataKuliah* mk = sistem.cariMataKuliah(unesc(k[1]));
            MataKuliah* pre = sistem.cariMataKuliah(unesc(k[2]));
            if (!mk || !pre || !mk->tambahPrasyarat(pre)) return gagal("prasyarat tidak valid");
        } else if (tag == "KUR") {
            int tahun = 0;
            if (!butuh(3) || !keInt(k[2], tahun)) return gagal("rekaman KUR salah");
            if (!sistem.tambahKurikulum(make_unique<Kurikulum>(unesc(k[1]), tahun))) return gagal("kurikulum kembar");
        } else if (tag == "KURMK") {
            int tahun = 0;
            if (!butuh(4) || !keInt(k[2], tahun)) return gagal("rekaman KURMK salah");
            Kurikulum* kur = cariKur(unesc(k[1]), tahun);
            MataKuliah* mk = sistem.cariMataKuliah(unesc(k[3]));
            if (!kur || !mk || !kur->tambahMataKuliah(mk)) return gagal("mata kuliah kurikulum tidak valid");
        } else if (tag == "RU") {
            int kap = 0;
            if (!butuh(5) || !keInt(k[3], kap)) return gagal("rekaman RU salah");
            auto jenis = (k[4] == "LAB") ? JenisRuangan::LAB : JenisRuangan::KELAS;
            if (!sistem.tambahRuangan(make_unique<Ruangan>(unesc(k[1]), unesc(k[2]), kap, jenis))) return gagal("ruangan kembar");
        } else if (tag == "USR") {
            if (!butuh(11)) return gagal("rekaman USR salah");
            const string niu = unesc(k[2]), nama = unesc(k[3]), email = unesc(k[4]);
            const string prodi = unesc(k[5]), hash = unesc(k[6]);
            unique_ptr<User> u;
            if (k[1] == "M") {
                int angkatan = 0, smt = 0, tahunKur = 0;
                if (!keInt(k[7], angkatan) || !keInt(k[8], smt)) return gagal("angkatan/semester salah");
                Kurikulum* kur = nullptr;
                if (!k[9].empty()) {
                    if (!keInt(k[10], tahunKur) || !(kur = cariKur(unesc(k[9]), tahunKur))) return gagal("kurikulum mahasiswa tidak ditemukan");
                }
                u = make_unique<Mahasiswa>(niu, nama, email, prodi, hash, angkatan, smt, kur, true);
            } else if (k[1] == "D") {
                u = make_unique<Dosen>(niu, nama, email, prodi, hash, true);
            } else if (k[1] == "A") {
                u = make_unique<Admin>(niu, nama, email, prodi, hash, &sistem, true);
            } else {
                return gagal("jenis pengguna tidak dikenal");
            }
            if (!sistem.tambahUser(std::move(u))) return gagal("nomor induk kembar");
        } else if (tag == "KEL") {
            int kuota = 0, totalPert = 0;
            if (!butuh(11) || !keInt(k[5], kuota) || !keInt(k[6], totalPert)) return gagal("rekaman KEL salah");
            MataKuliah* mk = sistem.cariMataKuliah(unesc(k[2]));
            Dosen* dosen = dynamic_cast<Dosen*>(sistem.cariUser(unesc(k[3])));
            if (!mk || !dosen) return gagal("mata kuliah/dosen kelas tidak ditemukan");
            Kelas* kelas = sistem.tambahKelas(make_unique<Kelas>(unesc(k[1]), mk, dosen, unesc(k[4]), kuota, totalPert));
            if (!kelas) return gagal("id kelas kembar");
            dosen->tambahKelasDiampu(kelas);
            if (!k[7].empty()) {
                int hari = 0, mulai = 0, selesai = 0;
                Ruangan* r = sistem.cariRuangan(unesc(k[7]));
                if (!r || !keInt(k[8], hari) || hari < 0 || hari > static_cast<int>(Hari::JUMAT) ||
                    !keInt(k[9], mulai) || !keInt(k[10], selesai)) return gagal("penempatan kelas salah");
                kelas->setPenempatan(r, Jadwal(static_cast<Hari>(hari), mulai, selesai));
                if (!r->pesan(kelas)) return gagal("penempatan ruangan bentrok/tidak valid");
            }
        } else if (tag == "KRS") {
            if (!butuh(5)) return gagal("rekaman KRS salah");
            Mahasiswa* m = cariMhs(unesc(k[1]));
            if (!m) return gagal("mahasiswa KRS tidak ditemukan");
            KRS* krs = m->buatKRS(unesc(k[2]));
            if (!krs) return gagal("KRS ganda untuk periode yang sama");
            for (const string& id : pecah(k[4], ';')) {
                Kelas* kl = sistem.cariKelas(unesc(id));
                if (!kl || !krs->pulihkan(kl)) return gagal("kelas pada KRS tidak valid atau kuota terlampaui");
            }
            krs->pulihkanStatus(k[3] == "1" ? StatusKRS::DISAHKAN : StatusKRS::DRAFT);
        } else if (tag == "PER") {
            int nomor = 0;
            long long tgl = 0;
            if (!butuh(6) || !keInt(k[2], nomor) || !keLL(k[3], tgl)) return gagal("rekaman PER salah");
            Kelas* kl = sistem.cariKelas(unesc(k[1]));
            if (!kl) return gagal("kelas pertemuan tidak ditemukan");
            for (const string& pasangan : pecah(k[5], ';')) {
                const vector<string> kv = pecah(pasangan, '=');
                if (kv.size() != 2) return gagal("daftar hadir salah");
                Mahasiswa* m = cariMhs(unesc(kv[0]));
                // peserta yang sudah membatalkan kelas dilewati (bukan kesalahan)
                if (m) kl->catatKehadiran(*m, nomor, kv[1] == "1");
            }
            for (Pertemuan* p : kl->getDaftarPertemuan()) {
                if (p->getNomor() == nomor) { p->setMateri(unesc(k[4])); p->setTanggal(static_cast<time_t>(tgl)); }
            }
        } else if (tag == "TGS") {
            long long deadline = 0;
            double bobot = 0;
            if (!butuh(7) || !keLL(k[5], deadline) || !keDouble(k[6], bobot)) return gagal("rekaman TGS salah");
            Kelas* kl = sistem.cariKelas(unesc(k[1]));
            if (!kl) return gagal("kelas tugas tidak ditemukan");
            kl->tambahTugas(make_unique<Tugas>(unesc(k[2]), unesc(k[3]), unesc(k[4]), static_cast<time_t>(deadline), bobot));
        } else if (tag == "PGP") {
            long long waktu = 0;
            double skor = 0;
            if (!butuh(8) || !keLL(k[5], waktu) || !keDouble(k[6], skor)) return gagal("rekaman PGP salah");
            Kelas* kl = sistem.cariKelas(unesc(k[1]));
            Mahasiswa* m = cariMhs(unesc(k[3]));
            if (!kl || !m) return gagal("kelas/mahasiswa pengumpulan tidak ditemukan");
            Tugas* tugas = nullptr;
            for (Tugas* t : kl->getDaftarTugas()) if (t->getId() == unesc(k[2])) tugas = t;
            if (!tugas) return gagal("tugas pengumpulan tidak ditemukan");
            Pengumpulan* pg = tugas->pulihkanPengumpulan(m, unesc(k[4]));
            if (!pg) return gagal("pengumpulan tidak dapat dipulihkan");
            pg->pulihkan(static_cast<time_t>(waktu), skor, k[7] == "1");
        } else if (tag == "PNG") {
            long long tgl = 0;
            if (!butuh(8) || !keLL(k[5], tgl)) return gagal("rekaman PNG salah");
            Kelas* kl = sistem.cariKelas(unesc(k[1]));
            if (!kl) return gagal("kelas pengumuman tidak ditemukan");
            Dosen* pembuat = dynamic_cast<Dosen*>(sistem.cariUser(unesc(k[7])));
            auto p = make_unique<Pengumuman>(unesc(k[2]), unesc(k[3]), unesc(k[4]), pembuat);
            p->pulihkan(static_cast<time_t>(tgl), k[6] == "1" ? StatusPengumuman::TERBIT : StatusPengumuman::DRAFT);
            kl->tambahPengumuman(std::move(p));
        } else if (tag == "NIL") {
            map<string, double> komponen;
            if (!butuh(5) || !bacaMap(k[4], komponen)) return gagal("rekaman NIL salah");
            Mahasiswa* m = cariMhs(unesc(k[1]));
            Kelas* kl = sistem.cariKelas(unesc(k[2]));
            if (!m || !kl) return gagal("mahasiswa/kelas nilai tidak ditemukan");
            Nilai* n = m->getTranskrip().ambilAtauBuatNilai(kl);
            for (const auto& [nama, skor] : komponen) {
                if (!n->updateKomponen(nama, skor)) return gagal("komponen nilai tidak valid: " + nama);
            }
            if (k[3] == "1" && !n->finalisasi()) return gagal("nilai tidak dapat difinalisasi");
        } else {
            return gagal("jenis rekaman tidak dikenal: " + tag);
        }
    }
    return true;
}
