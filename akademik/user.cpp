#include "univlink.h"
#include <cstdint>
#include <iostream>
#include <random>
#include <utility>

namespace {
    const uint32_t K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

    inline uint32_t rotr(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }

    // Menghasilkan 32 byte mentah (bukan hex) agar bisa langsung di-hash ulang.
    string sha256Mentah(const string& pesan) {
        uint32_t h[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                         0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

        // padding
        string data = pesan;
        const uint64_t panjangBit = static_cast<uint64_t>(pesan.size()) * 8;
        data.push_back(static_cast<char>(0x80));
        while (data.size() % 64 != 56) data.push_back('\0');
        for (int i = 7; i >= 0; --i) data.push_back(static_cast<char>((panjangBit >> (i * 8)) & 0xff));

        // proses tiap blok 64 byte
        for (size_t off = 0; off < data.size(); off += 64) {
            uint32_t w[64];
            for (int i = 0; i < 16; ++i) {
                w[i] = (uint32_t(uint8_t(data[off + 4 * i])) << 24) |
                       (uint32_t(uint8_t(data[off + 4 * i + 1])) << 16) |
                       (uint32_t(uint8_t(data[off + 4 * i + 2])) << 8) |
                       (uint32_t(uint8_t(data[off + 4 * i + 3])));
            }
            for (int i = 16; i < 64; ++i) {
                uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
                uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
                w[i] = w[i - 16] + s0 + w[i - 7] + s1;
            }

            uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g = h[6], hh = h[7];
            for (int i = 0; i < 64; ++i) {
                uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
                uint32_t ch = (e & f) ^ (~e & g);
                uint32_t t1 = hh + S1 + ch + K[i] + w[i];
                uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
                uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
                uint32_t t2 = S0 + maj;
                hh = g; g = f; f = e; e = d + t1;
                d = c; c = b; b = a; a = t1 + t2;
            }
            h[0] += a; h[1] += b; h[2] += c; h[3] += d;
            h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
        }

        // gabungkan keadaan akhir
        string digest;
        for (int i = 0; i < 8; ++i)
            for (int j = 3; j >= 0; --j) digest.push_back(static_cast<char>((h[i] >> (j * 8)) & 0xff));
        return digest;
    }

    string keHex(const string& bytes) {
        static const char* HEX = "0123456789abcdef";
        string hasil;
        hasil.reserve(bytes.size() * 2);
        for (unsigned char c : bytes) {
            hasil.push_back(HEX[c >> 4]);
            hasil.push_back(HEX[c & 0x0f]);
        }
        return hasil;
    }

    constexpr int RONDE = 5000;
    constexpr size_t PANJANG_MIN_PASSWORD = 6;

    string hashDenganSalt(const string& salt, const string& password) {
        string digest = sha256Mentah(salt + password);
        for (int i = 1; i < RONDE; ++i) digest = sha256Mentah(digest + salt + password);
        return keHex(digest);
    }

    string buatSalt() {
        random_device rd;
        mt19937_64 gen((static_cast<uint64_t>(rd()) << 32) ^ rd() ^
                       static_cast<uint64_t>(time(nullptr)));
        uint64_t r = gen();
        string bytes;
        for (int i = 7; i >= 0; --i) bytes.push_back(static_cast<char>((r >> (i * 8)) & 0xff));
        return keHex(bytes);
    }

    bool samaAman(const string& a, const string& b) {
        if (a.size() != b.size()) return false;
        unsigned char selisih = 0;
        for (size_t i = 0; i < a.size(); ++i) selisih |= static_cast<unsigned char>(a[i] ^ b[i]);
        return selisih == 0;
    }
}  
string sha256Hex(const string& pesan) { return keHex(sha256Mentah(pesan)); }
string User::hashPassword(const string& password) {
    const string salt = buatSalt();
    return salt + "$" + hashDenganSalt(salt, password);
}

User::User(string nomorInduk, string nama, string email, string prodi, const string& password, bool passwordSudahHash)
    : nomorInduk(move(nomorInduk)),
      nama(move(nama)),
      email(move(email)),
      prodi(move(prodi)),
      passwordHash(passwordSudahHash ? password : hashPassword(password)) {}

string User::getNomorInduk() const { return nomorInduk; }
string User::getNama() const { return nama; }
string User::getEmail() const { return email; }
string User::getProdi() const { return prodi; }

bool User::verifikasiPassword(const string& password) const {
    const size_t pos = passwordHash.find('$');
    if (pos == string::npos) return false;
    const string salt = passwordHash.substr(0, pos);
    const string hashTersimpan = passwordHash.substr(pos + 1);
    return samaAman(hashDenganSalt(salt, password), hashTersimpan);
}

bool User::gantiPassword(const string& lama, const string& baru) {
    if (!verifikasiPassword(lama)) return false;
    if (baru.size() < PANJANG_MIN_PASSWORD) return false;
    passwordHash = hashPassword(baru);
    return true;
}


void User::cetakHeader() const { 
    cout << "=== " << getPeran() << " ===\n"<< nama << " (" << nomorInduk << ")  |  Prodi: " << prodi << "\n";
}
