#include "ecdsa.h"
#include "hash160.h"
#include "sha256.h"
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <sstream>
#include <iomanip>
#include <stdexcept>

std::vector<uint8_t> ECDSA::hexToBytes(const std::string& hex) {
    std::vector<uint8_t> b;
    for (size_t i=0;i+1<hex.size();i+=2)
        b.push_back((uint8_t)std::stoi(hex.substr(i,2),nullptr,16));
    return b;
}
std::string ECDSA::bytesToHex(const std::vector<uint8_t>& b) {
    std::stringstream ss;
    for (uint8_t x:b) ss<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)x;
    return ss.str();
}

KeyPair ECDSA::generateKeyPair() {
    EC_KEY* key=EC_KEY_new_by_curve_name(NID_secp256k1);
    EC_KEY_generate_key(key);
    const BIGNUM* priv=EC_KEY_get0_private_key(key);
    uint8_t privBuf[32]={};
    BN_bn2binpad(priv,privBuf,32);
    const EC_POINT* pub=EC_KEY_get0_public_key(key);
    const EC_GROUP* grp=EC_KEY_get0_group(key);
    uint8_t pubBuf[33];
    EC_POINT_point2oct(grp,pub,POINT_CONVERSION_COMPRESSED,pubBuf,33,nullptr);
    EC_KEY_free(key);
    KeyPair kp;
    kp.privateKey=bytesToHex({privBuf,privBuf+32});
    kp.publicKey =bytesToHex({pubBuf, pubBuf+33});
    return kp;
}

std::string ECDSA::derivePublicKey(const std::string& privHex) {
    auto pb=hexToBytes(privHex);
    EC_KEY* key=EC_KEY_new_by_curve_name(NID_secp256k1);
    const EC_GROUP* grp=EC_KEY_get0_group(key);
    BIGNUM* priv=BN_bin2bn(pb.data(),(int)pb.size(),nullptr);
    EC_KEY_set_private_key(key,priv);
    EC_POINT* pub=EC_POINT_new(grp);
    EC_POINT_mul(grp,pub,priv,nullptr,nullptr,nullptr);
    EC_KEY_set_public_key(key,pub);
    uint8_t buf[33];
    EC_POINT_point2oct(grp,pub,POINT_CONVERSION_COMPRESSED,buf,33,nullptr);
    EC_POINT_free(pub); BN_free(priv); EC_KEY_free(key);
    return bytesToHex({buf,buf+33});
}

Signature ECDSA::sign(const std::string& privHex,const std::string& hashHex) {
    auto pb=hexToBytes(privHex);
    auto hb=hexToBytes(hashHex);
    hb.resize(32,0);
    EC_KEY* key=EC_KEY_new_by_curve_name(NID_secp256k1);
    const EC_GROUP* grp=EC_KEY_get0_group(key);
    BIGNUM* priv=BN_bin2bn(pb.data(),(int)pb.size(),nullptr);
    EC_KEY_set_private_key(key,priv);
    EC_POINT* pub=EC_POINT_new(grp);
    EC_POINT_mul(grp,pub,priv,nullptr,nullptr,nullptr);
    EC_KEY_set_public_key(key,pub);
    ECDSA_SIG* sig=ECDSA_do_sign(hb.data(),32,key);
    Signature result;
    if(sig){
        const BIGNUM *r,*s;
        ECDSA_SIG_get0(sig,&r,&s);
        uint8_t rb[32]={},sb[32]={};
        BN_bn2binpad(r,rb,32); BN_bn2binpad(s,sb,32);
        result.r=bytesToHex({rb,rb+32});
        result.s=bytesToHex({sb,sb+32});
        unsigned char* der=nullptr;
        int dl=i2d_ECDSA_SIG(sig,&der);
        if(dl>0){result.der={der,der+dl};OPENSSL_free(der);}
        ECDSA_SIG_free(sig);
    }
    EC_POINT_free(pub); BN_free(priv); EC_KEY_free(key);
    return result;
}

bool ECDSA::verify(const std::string& pubHex,const std::string& hashHex,const Signature& sig) {
    if(sig.r.empty()||sig.s.empty()) return false;
    auto pb=hexToBytes(pubHex);
    auto hb=hexToBytes(hashHex); hb.resize(32,0);
    EC_KEY* key=EC_KEY_new_by_curve_name(NID_secp256k1);
    const EC_GROUP* grp=EC_KEY_get0_group(key);
    EC_POINT* pub=EC_POINT_new(grp);
    EC_POINT_oct2point(grp,pub,pb.data(),pb.size(),nullptr);
    EC_KEY_set_public_key(key,pub);
    ECDSA_SIG* ecSig=ECDSA_SIG_new();
    BIGNUM* r=BN_bin2bn(hexToBytes(sig.r).data(),32,nullptr);
    BIGNUM* s=BN_bin2bn(hexToBytes(sig.s).data(),32,nullptr);
    ECDSA_SIG_set0(ecSig,r,s);
    int ok=ECDSA_do_verify(hb.data(),32,ecSig,key);
    ECDSA_SIG_free(ecSig); EC_POINT_free(pub); EC_KEY_free(key);
    return ok==1;
}

std::string ECDSA::publicKeyToAddress(const std::string& pubHex) {
    auto pb=hexToBytes(pubHex);
    return Hash160::toAddress(pb,0x3C); // 0x3C → starts with 'R'
}
