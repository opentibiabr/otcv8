/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "crypt.h"
#include <framework/stdext/math.h>
#include <framework/core/logger.h>
#include <framework/core/resourcemanager.h>
#include <framework/platform/platform.h>
#include <framework/core/application.h>

#ifndef __EMSCRIPTEN__
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#endif
#include <zlib.h>

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static inline bool is_base64(unsigned char c) { return (isalnum(c) || (c == '+') || (c == '/')); }

Crypt g_crypt;

Crypt::Crypt()
{
#ifndef __EMSCRIPTEN__
    m_rsa = RSA_new();
#endif
}

Crypt::~Crypt()
{
#ifndef __EMSCRIPTEN__
    RSA_free(m_rsa);
#endif
}

std::string Crypt::base64Encode(const std::string& decoded_string)
{
    std::string ret;
    int i = 0;
    int j = 0;
    uint8 char_array_3[3];
    uint8 char_array_4[4];
    int pos = 0;
    int len = decoded_string.size();

    while(len--) {
        char_array_3[i++] = decoded_string[pos++];
        if(i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if(i) {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for(j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while((i++ < 3))
            ret += '=';
    }

    return ret;
}

std::string Crypt::base64Decode(const std::string& encoded_string)
{
    int len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    uint8 char_array_4[4], char_array_3[3];
    std::string ret;

    while(len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if(i ==4) {
            for(i = 0; i <4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for(i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if(i) {
        for(j = i; j <4; j++)
            char_array_4[j] = 0;

        for(j = 0; j <4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for(j = 0; (j < i - 1); j++)
            ret += char_array_3[j];
    }

    return ret;
}

std::string Crypt::xorCrypt(const std::string& buffer, const std::string& key)
{
    std::string out;
    out.resize(buffer.size());
    size_t i, j=0;
    for(i=0;i<buffer.size();++i) {
        out[i] = buffer[i] ^ key[j++];
        if(j >= key.size())
            j = 0;
    }
    return out;
}

std::string Crypt::genUUID()
{
    std::random_device rd;
    auto seed_data = std::array<int, std::mt19937::state_size> {};
    std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
    std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
    std::mt19937 generator(seq);

    return to_string(uuids::uuid_random_generator{ generator }());
}

bool Crypt::setMachineUUID(std::string uuidstr)
{
    if (uuidstr.empty())
        return false;

    uuidstr = _decrypt(uuidstr, false);

    if (uuidstr.length() != 36)
        return false;

    m_machineUUID = uuids::uuid::from_string(uuidstr).value();

    return true;
}

std::string Crypt::getMachineUUID()
{
    if (m_machineUUID.is_nil()) {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size> {};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937 generator(seq);

        m_machineUUID = uuids::uuid_random_generator{ generator }();
    }

    return _encrypt(to_string(m_machineUUID), false);
}

std::string Crypt::getCryptKey(bool useMachineUUID)
{
    constexpr std::hash<uuids::uuid> uuid_hasher;
    const uuids::uuid uuid = useMachineUUID ? m_machineUUID : uuids::uuid();
    const uuids::uuid u = uuids::uuid_name_generator(uuid)
        (g_app.getCompactName() + g_platform.getCPUName() + g_platform.getOSName());
    const std::size_t hash = uuid_hasher(u);

    std::string key;
    key.assign((const char*)&hash, sizeof(hash));
    return key;
}


std::string Crypt::_encrypt(const std::string& decrypted_string, bool useMachineUUID)
{
    std::string tmp = std::string("0000") + decrypted_string;
    uint32 sum = stdext::adler32((const uint8*)decrypted_string.c_str(), decrypted_string.size());
    stdext::writeULE32((uint8*)&tmp[0], sum);
    std::string encrypted = base64Encode(xorCrypt(tmp, getCryptKey(useMachineUUID)));
    return encrypted;
}

std::string Crypt::_decrypt(const std::string& encrypted_string, bool useMachineUUID)
{
    std::string decoded = base64Decode(encrypted_string);
    std::string tmp = xorCrypt(decoded, getCryptKey(useMachineUUID));
    if(tmp.length() >= 4) {
        uint32 readsum = stdext::readULE32((const uint8*)tmp.c_str());
        std::string decrypted_string = tmp.substr(4);
        uint32 sum = stdext::adler32((const uint8*)decrypted_string.c_str(), decrypted_string.size());
        if(readsum == sum)
            return decrypted_string;
    }
    return std::string();
}

std::string Crypt::md5Encode(const std::string& decoded_string, bool upperCase)
{
#ifndef __EMSCRIPTEN__
    MD5_CTX c;
    MD5_Init(&c);
    MD5_Update(&c, decoded_string.c_str(), decoded_string.length());

    uint8_t md[MD5_DIGEST_LENGTH];
    MD5_Final(md, &c);

    char output[(MD5_DIGEST_LENGTH << 1) + 1];
    for(int32_t i = 0; i < (int32_t)sizeof(md); ++i)
        sprintf(output + (i << 1), "%.2X", md[i]);

    std::string result = output;
    if(upperCase)
        return result;

    std::transform(result.begin(), result.end(), result.begin(), tolower);
    return result;
#else
    return "";
#endif
}

std::string Crypt::sha1Encode(const std::string& decoded_string, bool upperCase)
{
#ifndef __EMSCRIPTEN__
    SHA_CTX c;
    SHA1_Init(&c);
    SHA1_Update(&c, decoded_string.c_str(), decoded_string.length());

    uint8_t md[SHA_DIGEST_LENGTH];
    SHA1_Final(md, &c);

    char output[(SHA_DIGEST_LENGTH << 1) + 1];
    for(int32_t i = 0; i < (int32_t)sizeof(md); ++i)
        sprintf(output + (i << 1), "%.2X", md[i]);

    std::string result = output;
    if(upperCase)
        return result;

    std::transform(result.begin(), result.end(), result.begin(), tolower);
    return result;
#else
    return "";
#endif
}

std::string Crypt::sha256Encode(const std::string& decoded_string, bool upperCase)
{
#ifndef __EMSCRIPTEN__
    SHA256_CTX c;
    SHA256_Init(&c);
    SHA256_Update(&c, decoded_string.c_str(), decoded_string.length());

    uint8_t md[SHA256_DIGEST_LENGTH];
    SHA256_Final(md, &c);

    char output[(SHA256_DIGEST_LENGTH << 1) + 1];
    for(int32_t i = 0; i < (int32_t)sizeof(md); ++i)
        sprintf(output + (i << 1), "%.2X", md[i]);

    std::string result = output;
    if(upperCase)
        return result;

    std::transform(result.begin(), result.end(), result.begin(), tolower);
    return result;
#else
    return "";
#endif
}

std::string Crypt::sha512Encode(const std::string& decoded_string, bool upperCase)
{
#ifndef __EMSCRIPTEN__
    SHA512_CTX c;
    SHA512_Init(&c);
    SHA512_Update(&c, decoded_string.c_str(), decoded_string.length());

    uint8_t md[SHA512_DIGEST_LENGTH];
    SHA512_Final(md, &c);

    char output[(SHA512_DIGEST_LENGTH << 1) + 1];
    for(int32_t i = 0; i < (int32_t)sizeof(md); ++i)
        sprintf(output + (i << 1), "%.2X", md[i]);

    std::string result = output;
    if(upperCase)
        return result;

    std::transform(result.begin(), result.end(), result.begin(), tolower);
    return result;
#else
    return "";
#endif
}

std::string Crypt::crc32(const std::string& decoded_string, bool upperCase)
{
    uint32_t crc = ::crc32(0, Z_NULL, 0);
    crc = ::crc32(crc, (const Bytef*)decoded_string.c_str(), decoded_string.size());
    std::string result = stdext::dec_to_hex(crc);
    if (upperCase)
        std::transform(result.begin(), result.end(), result.begin(), toupper);
    else
        std::transform(result.begin(), result.end(), result.begin(), tolower);
    return result;
}


void Crypt::rsaGenerateKey(int bits, int e)
{
    // disabled because new OpenSSL changes broke
    /*
    RSA *rsa = RSA_new();
    BIGNUM *ebn = BN_new();
    BN_set_word(ebn, e);
    RSA_generate_key_ex(rsa, bits, ebn, nullptr);
    g_logger.info(stdext::format("%d bits (%d bytes) RSA key generated", bits, bits / 8));
    g_logger.info(std::string("p = ") + BN_bn2dec(m_rsa->p));
    g_logger.info(std::string("q = ") + BN_bn2dec(m_rsa->q));
    g_logger.info(std::string("d = ") + BN_bn2dec(m_rsa->d));
    g_logger.info(std::string("n = ") + BN_bn2dec(m_rsa->n));
    g_logger.info(std::string("e = ") + BN_bn2dec(m_rsa->e));
    BN_clear_free(ebn);
    RSA_free(rsa);
    */
}

void Crypt::rsaSetPublicKey(const std::string& n, const std::string& e)
{
#ifndef __EMSCRIPTEN__
#if OPENSSL_VERSION_NUMBER < 0x10100005L
    BN_dec2bn(&m_rsa->n, n.c_str());
    BN_dec2bn(&m_rsa->e, e.c_str());
    // clear rsa cache
    if(m_rsa->_method_mod_n) {
        BN_MONT_CTX_free(m_rsa->_method_mod_n);
        m_rsa->_method_mod_n = nullptr;
    }
#else
    BIGNUM *bn = nullptr, *be = nullptr;
    BN_dec2bn(&bn, n.c_str());
    BN_dec2bn(&be, e.c_str());
    RSA_set0_key(m_rsa, bn, be, nullptr);
#endif
#endif
}

void Crypt::rsaSetPrivateKey(const std::string& p, const std::string& q, const std::string& d)
{
#ifndef __EMSCRIPTEN__
#if OPENSSL_VERSION_NUMBER < 0x10100005L
    BN_dec2bn(&m_rsa->p, p.c_str());
    BN_dec2bn(&m_rsa->q, q.c_str());
    BN_dec2bn(&m_rsa->d, d.c_str());
    // clear rsa cache
    if(m_rsa->_method_mod_p) {
        BN_MONT_CTX_free(m_rsa->_method_mod_p);
        m_rsa->_method_mod_p = nullptr;
    }
    if(m_rsa->_method_mod_q) {
        BN_MONT_CTX_free(m_rsa->_method_mod_q);
        m_rsa->_method_mod_q = nullptr;
    }
#else
    BIGNUM *bp = nullptr, *bq = nullptr, *bd = nullptr;
    BN_dec2bn(&bp, p.c_str());
    BN_dec2bn(&bq, q.c_str());
    BN_dec2bn(&bd, d.c_str());
    RSA_set0_key(m_rsa, nullptr, nullptr, bd);
    RSA_set0_factors(m_rsa, bp, bq);
#endif
#endif
}

bool Crypt::rsaCheckKey()
{
#ifndef __EMSCRIPTEN__
    // only used by server, that sets both public and private
    if(RSA_check_key(m_rsa)) {
        BN_CTX *ctx = BN_CTX_new();
        BN_CTX_start(ctx);

        BIGNUM *r1 = BN_CTX_get(ctx), *r2 = BN_CTX_get(ctx);
#if OPENSSL_VERSION_NUMBER < 0x10100005L
        BN_mod(m_rsa->dmp1, m_rsa->d, r1, ctx);
        BN_mod(m_rsa->dmq1, m_rsa->d, r2, ctx);
        BN_mod_inverse(m_rsa->iqmp, m_rsa->q, m_rsa->p, ctx);
#else
        const BIGNUM *dmp1_c = nullptr, *d = nullptr, *dmq1_c = nullptr, *iqmp_c = nullptr, *q = nullptr, *p = nullptr;

        RSA_get0_key(m_rsa, nullptr, nullptr, &d);
        RSA_get0_factors(m_rsa, &p, &q);
        RSA_get0_crt_params(m_rsa, &dmp1_c, &dmq1_c, &iqmp_c);

        BIGNUM *dmp1 = BN_dup(dmp1_c), *dmq1 = BN_dup(dmq1_c), *iqmp = BN_dup(iqmp_c);

        BN_mod(dmp1, d, r1, ctx);
        BN_mod(dmq1, d, r2, ctx);
        BN_mod_inverse(iqmp, q, p, ctx);
        RSA_set0_crt_params(m_rsa, dmp1, dmq1, iqmp);
#endif
        return true;
    }
    else {
        ERR_load_crypto_strings();
        g_logger.error(stdext::format("RSA check failed - %s", ERR_error_string(ERR_get_error(), nullptr)));
        return false;
    }
#else
    return false;
#endif
}

bool Crypt::rsaEncrypt(unsigned char *msg, int size)
{
#ifndef __EMSCRIPTEN__
    if(size != RSA_size(m_rsa))
        return false;
    return RSA_public_encrypt(size, msg, msg, m_rsa, RSA_NO_PADDING) != -1;
#else
    return false;
#endif
}

bool Crypt::rsaDecrypt(unsigned char *msg, int size)
{
#ifndef __EMSCRIPTEN__
    if(size != RSA_size(m_rsa))
        return false;
    return RSA_private_decrypt(size, msg, msg, m_rsa, RSA_NO_PADDING) != -1;
#else
    return false;
#endif
}

int Crypt::rsaGetSize()
{
#ifndef __EMSCRIPTEN__
    return RSA_size(m_rsa);
#else
    return 0;
#endif
}

#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[(p&3)^e] ^ z)))
#ifdef WITH_ENCRYPTION
void Crypt::bencrypt(uint8_t* buffer, int len, uint64_t k) {
    uint32_t const key[4] = { (uint32_t)(k >> 32), (uint32_t)(k & 0xFFFFFFFF), 0xDEADDEAD, 0xB00BEEEF };
    uint32_t y, z, sum;
    uint32_t *v = (uint32_t*)buffer;
    unsigned rounds, e;
    int p, n = (len - len % 4) / 4;
    if (n < 2)
        return;
    rounds = 6 + 52 / n;
    sum = 0;
    z = v[n - 1];
    do {
        sum += DELTA;
        e = (sum >> 2) & 3;
        for (p = 0; p < n - 1; p++) {
            y = v[p + 1];
            z = v[p] += MX;
        }
        y = v[0];
        z = v[n - 1] += MX;
    } while (--rounds);
}
#endif

void Crypt::bdecrypt(uint8_t* buffer, int len, uint64_t k) {
    uint32_t const key[4] = { (uint32_t)(k >> 32), (uint32_t)(k & 0xFFFFFFFF), 0xDEADDEAD, 0xB00BEEEF };
    uint32_t y, z, sum;
    uint32_t *v = (uint32_t*)buffer;
    unsigned p, rounds, e;
    int n = (len - len % 4) / 4;
    if (n < 2)
        return;
    rounds = 6 + 52 / n;
    sum = rounds * DELTA;
    y = v[0];
    do {
        e = (sum >> 2) & 3;
        for (p = n - 1; p > 0; p--) {
            z = v[p - 1];
            y = v[p] -= MX;
        }
        z = v[n - 1];
        y = v[0] -= MX;
        sum -= DELTA;
    } while (--rounds);
}
