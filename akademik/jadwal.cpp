#include "univlink.h"
#include <iomanip>
#include <sstream>
using namespace std;

string hariKeString(Hari h) {
    switch (h) {
        case Hari::SENIN:  return "Senin";
        case Hari::SELASA: return "Selasa";
        case Hari::RABU:   return "Rabu";
        case Hari::KAMIS:  return "Kamis";
        case Hari::JUMAT:  return "Jumat";
    }
    return "?";
}

namespace {
    int keMenit(int hhmm) {
        return (hhmm / 100) * 60 + (hhmm % 100);
    }
    bool jamSah(int hhmm) {
        return hhmm >= 0 && hhmm <= 2359 && (hhmm % 100) < 60;
    }
}

Jadwal::Jadwal(Hari hari, int jamMulai, int jamSelesai) : hari(hari), jamMulai(jamMulai), jamSelesai(jamSelesai) {}

Hari Jadwal::getHari() const { return hari; }
int Jadwal::getJamMulai() const { return jamMulai; }
int Jadwal::getJamSelesai() const { return jamSelesai; }

bool Jadwal::valid() const {
    if (!jamSah(jamMulai) || !jamSah(jamSelesai)) return false;
    return keMenit(jamMulai) < keMenit(jamSelesai);
}

bool Jadwal::bentrokDengan(const Jadwal& lain) const {
    if (!valid() || !lain.valid()) return false;
    if (hari != lain.hari) return false;
    const int mulaiA = keMenit(jamMulai), selesaiA = keMenit(jamSelesai);
    const int mulaiB = keMenit(lain.jamMulai), selesaiB = keMenit(lain.jamSelesai);
    return mulaiA < selesaiB && mulaiB < selesaiA;
}

string Jadwal::toString() const {
    auto format = [](int hhmm) {
        ostringstream os;
        os << setfill('0') << setw(2) << (hhmm / 100) << ':' << setw(2) << (hhmm % 100);
        return os.str();
    };
    return hariKeString(hari) + " " + format(jamMulai) + "-" + format(jamSelesai);
}
