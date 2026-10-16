//   Huruf  Bobot  Rentang skor
//   A      4.00   90    - 100
//   A-     3.75   85    - 89.99
//   A/B    3.50   80    - 84.99
//   B+     3.25   75    - 79.99
//   B      3.00   70    - 74.99
//   B-     2.75   65    - 69.99
//   B/C    2.50   60    - 64.99
//   C+     2.25   55    - 59.99
//   C      2.00   50    - 54.99
//   C-     1.75   45    - 49.99
//   D      1.00   40    - 44.99
//   E      0.00   0     - 39.99

#include "univlink.h"
#include <cmath>
namespace {

struct BarisKonversi {
    double batasBawah;
    NilaiHuruf huruf;
    double bobot;
    const char* teks;
};
const BarisKonversi TABEL[] = {
    {90.0, NilaiHuruf::A,      4.00, "A"},
    {85.0, NilaiHuruf::A_MIN,  3.75, "A-"},
    {80.0, NilaiHuruf::A_B,    3.50, "A/B"},
    {75.0, NilaiHuruf::B_PLUS, 3.25, "B+"},
    {70.0, NilaiHuruf::B,      3.00, "B"},
    {65.0, NilaiHuruf::B_MIN,  2.75, "B-"},
    {60.0, NilaiHuruf::B_C,    2.50, "B/C"},
    {55.0, NilaiHuruf::C_PLUS, 2.25, "C+"},
    {50.0, NilaiHuruf::C,      2.00, "C"},
    {45.0, NilaiHuruf::C_MIN,  1.75, "C-"},
    {40.0, NilaiHuruf::D,      1.00, "D"},
    {0.0,  NilaiHuruf::E,      0.00, "E"},
};

const BarisKonversi& cariBaris(NilaiHuruf h) {
    for (const auto& b : TABEL) {
        if (b.huruf == h) return b;
    }
    return TABEL[sizeof(TABEL) / sizeof(TABEL[0]) - 1];  // tidak tercapai; E
}

} 

NilaiHuruf KonversiNilai::skorKeHuruf(double skor) {
    if (!(skor > 0.0)) return NilaiHuruf::E;       // 0, negatif, dan NaN -> E
    skor = std::round(skor * 100.0) / 100.0;       // 2 desimal
    for (const auto& b : TABEL) {
        if (skor >= b.batasBawah) return b.huruf;
    }
    return NilaiHuruf::E;
}

double KonversiNilai::hurufKeBobot(NilaiHuruf h) { return cariBaris(h).bobot; }

string KonversiNilai::hurufKeString(NilaiHuruf h) { return cariBaris(h).teks; }

bool KonversiNilai::isLulus(NilaiHuruf h) { return hurufKeBobot(h) > 0.0; }
