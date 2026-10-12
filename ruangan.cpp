#include "univlink.h"
#include <algorithm>

string jenisRuanganKeString(JenisRuangan j) {
    switch (j) {
        case JenisRuangan::KELAS: return "Kelas";
        case JenisRuangan::LAB:   return "Lab";
    }
    return "?";
}

Ruangan::Ruangan(const string& kode, const string& gedung, int kapasitas, JenisRuangan jenis)
    : kodeRuangan(kode), gedung(gedung), kapasitas(kapasitas < 0 ? 0 : kapasitas), jenis(jenis) {}

const string& Ruangan::getKode() const { return kodeRuangan; }
const string& Ruangan::getGedung() const { return gedung; }
int Ruangan::getKapasitas() const { return kapasitas; }
JenisRuangan Ruangan::getJenis() const { return jenis; }

bool Ruangan::tersedia(const Jadwal& j, const string& periode) const {
    for (const Kelas* k : terjadwal) {
        if (!k || !k->punyaJadwal()) continue;
        if (!periode.empty() && k->getPeriode() != periode) continue;
        if (k->getJadwal().bentrokDengan(j)) return false;
    }
    return true;
}

bool Ruangan::pesan(Kelas* kelas) {
    if (!kelas) return false;
    if (find(terjadwal.begin(), terjadwal.end(), kelas) != terjadwal.end()) return false;
    if (!kelas->punyaJadwal()) return false;
    if (kapasitas < kelas->getKuota()) return false;
    if (!tersedia(kelas->getJadwal(), kelas->getPeriode())) return false;

    terjadwal.push_back(kelas);
    return true;
}

void Ruangan::lepas(Kelas* kelas) {
    terjadwal.erase(remove(terjadwal.begin(), terjadwal.end(), kelas), terjadwal.end());
}
