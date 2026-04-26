#include "ripemd160.h"
#include <cstring>
#include <sstream>
#include <iomanip>

// RIPEMD-160 — as used in Bitcoin address derivation
// Reference: https://homes.esat.kuleuven.be/~bosselae/ripemd160.html

const uint32_t RIPEMD160::K1[5] = {0x00000000,0x5A827999,0x6ED9EBA1,0x8F1BBCDC,0xA953FD4E};
const uint32_t RIPEMD160::K2[5] = {0x50A28BE6,0x5C4DD124,0x6D703EF3,0x7A6D76E9,0x00000000};

const int RIPEMD160::R1[80] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
    7,4,13,1,10,6,15,3,12,0,9,5,2,14,11,8,
    3,10,14,4,9,15,8,1,2,7,0,6,13,11,5,12,
    1,9,11,10,0,8,12,4,13,3,7,15,14,5,6,2,
    4,0,5,9,7,12,2,10,14,1,3,8,11,6,15,13
};
const int RIPEMD160::R2[80] = {
    5,14,7,0,9,2,11,4,13,6,15,8,1,10,3,12,
    6,11,3,7,0,13,5,10,14,15,8,12,4,9,1,2,
    15,5,1,3,7,14,6,9,11,8,12,2,10,0,4,13,
    8,6,4,1,3,11,15,0,5,12,2,13,9,7,10,14,
    12,15,10,4,1,5,8,7,6,2,13,14,0,3,9,11
};
const int RIPEMD160::S1[80] = {
    11,14,15,12,5,8,7,9,11,13,14,15,6,7,9,8,
    7,6,8,13,11,9,7,15,7,12,15,9,11,7,13,12,
    11,13,6,7,14,9,13,15,14,8,13,6,5,12,7,5,
    11,12,14,15,14,15,9,8,9,14,5,6,8,6,5,12,
    9,15,5,11,6,8,13,12,5,12,13,14,11,8,5,6
};
const int RIPEMD160::S2[80] = {
    8,9,9,11,13,15,15,5,7,7,8,11,14,14,12,6,
    9,13,15,7,12,8,9,11,7,7,12,7,6,15,13,11,
    9,7,15,11,8,6,6,14,12,13,5,14,13,13,7,5,
    15,5,8,11,14,14,6,14,6,9,12,9,12,5,15,8,
    8,5,12,9,12,5,14,6,8,13,6,5,15,13,11,11
};

uint32_t RIPEMD160::rol(uint32_t x, int n) { return (x<<n)|(x>>(32-n)); }

uint32_t RIPEMD160::f(int j, uint32_t x, uint32_t y, uint32_t z) {
    if      (j <16) return x^y^z;
    else if (j <32) return (x&y)|(~x&z);
    else if (j <48) return (x|~y)^z;
    else if (j <64) return (x&z)|(y&~z);
    else            return x^(y|~z);
}

std::string RIPEMD160::toHex(const uint8_t* d, size_t len) {
    std::stringstream ss;
    for (size_t i=0;i<len;i++)
        ss<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)d[i];
    return ss.str();
}

std::string RIPEMD160::hash(const std::string& input) {
    auto res = hashBytes(std::vector<uint8_t>(input.begin(), input.end()));
    return toHex(res.data(), res.size());
}

std::vector<uint8_t> RIPEMD160::hashBytes(const std::vector<uint8_t>& input) {
    // Padding (same as MD4/SHA1)
    std::vector<uint8_t> msg = input;
    uint64_t bitLen = (uint64_t)msg.size() * 8;
    msg.push_back(0x80);
    while (msg.size() % 64 != 56) msg.push_back(0x00);
    for (int i=0;i<8;i++) msg.push_back((uint8_t)(bitLen>>(8*i)));

    // Initial hash values
    uint32_t h[5] = {0x67452301,0xEFCDAB89,0x98BADCFE,0x10325476,0xC3D2E1F0};

    for (size_t blk=0; blk<msg.size(); blk+=64) {
        uint32_t X[16];
        for (int i=0;i<16;i++) {
            X[i] = (uint32_t)msg[blk+i*4]
                 | ((uint32_t)msg[blk+i*4+1]<<8)
                 | ((uint32_t)msg[blk+i*4+2]<<16)
                 | ((uint32_t)msg[blk+i*4+3]<<24);
        }

        uint32_t al=h[0],bl=h[1],cl=h[2],dl=h[3],el=h[4];
        uint32_t ar=h[0],br=h[1],cr=h[2],dr=h[3],er=h[4];

        for (int j=0;j<80;j++) {
            uint32_t T = rol(al + f(j,bl,cl,dl) + X[R1[j]] + K1[j/16], S1[j]) + el;
            al=el; el=dl; dl=rol(cl,10); cl=bl; bl=T;
            T = rol(ar + f(79-j,br,cr,dr) + X[R2[j]] + K2[j/16], S2[j]) + er;
            ar=er; er=dr; dr=rol(cr,10); cr=br; br=T;
        }

        uint32_t T = h[1]+cl+dr;
        h[1]=h[2]+dl+er; h[2]=h[3]+el+ar;
        h[3]=h[4]+al+br; h[4]=h[0]+bl+cr; h[0]=T;
    }

    std::vector<uint8_t> result(20);
    for (int i=0;i<5;i++) {
        result[i*4+0]=(uint8_t)(h[i]);
        result[i*4+1]=(uint8_t)(h[i]>>8);
        result[i*4+2]=(uint8_t)(h[i]>>16);
        result[i*4+3]=(uint8_t)(h[i]>>24);
    }
    return result;
}
