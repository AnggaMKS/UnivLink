// Alur: 
// ->muat data dari folder "data" 
//-> (bila kosong) isi data contoh/seed 
// -> jalankan layar login 
// -> simpan. 
#include "cli/cli.h"
int main() {
    PenyimpananFile berkas("data");
    SistemAkademik sistem;
    sistem.setPenyimpanan(&berkas);

    if (!sistem.muatData()) {
        cerr << "Gagal membaca folder data/. Perbaiki atau pindahkan folder tersebut, lalu jalankan ulang.\n"
                "(Data TIDAK ditimpa agar tidak ada yang hilang.)\n";
        return 1;
    }
    if (sistem.getDaftarUser().empty()) {
        isiDataAwal(sistem);
        if (!sistem.simpanData()) cerr << "Peringatan: data contoh gagal disimpan.\n";
    }

    try {
        jalankanAplikasi(sistem);
    } catch (const InputBerakhir&) {
        cout << "\n(Input berakhir)\n";
    }
    if (!sistem.simpanData()) cerr << "Peringatan: data gagal disimpan.\n";
    cout << "Sampai jumpa.\n";
    return 0;
}
