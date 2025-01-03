#pragma once

#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>

namespace security
{
std::string Base64Encode(const std::string &str);
std::string Base64Encode(const std::vector<uint8_t> &vec);

std::vector<uint8_t> Base64Decode(const std::string &str);

std::string SHA1Hash(const std::vector<uint8_t> &data);
std::string SHA1Hash(const std::string &data);

std::string SHA256Hash(const std::vector<uint8_t> &data);
std::string SHA256Hash(const std::string &data);
std::string SHA256HashFile(const std::string &filePath);
} // namespace security