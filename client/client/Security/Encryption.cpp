#include "..\Includes.hpp"

namespace security
{
std::string Base64Encode(const std::vector<uint8_t> &vec)
{
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);

    bool done = false;

    while (!done)
    {
        const int encodedSize = BIO_write(bio, vec.data(), static_cast<int>(vec.size()));
        if (encodedSize <= 0)
        {
            if (BIO_should_retry(bio))
            {
                continue;
            }
            else
            {
                throw std::runtime_error("Failed to encode Base64 string!");
            }
        }
        else
        {
            done = true;
        }
    }

    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string encoded(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);

    return encoded;
}

std::string Base64Encode(const std::string &str)
{
    return Base64Encode(std::vector<uint8_t>(str.begin(), str.end()));
}

std::vector<uint8_t> Base64Decode(const std::string &input)
{
    BIO *bio, *b64;
    char *buffer = new char[input.size()];
    memset(buffer, 0, input.size());

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(input.data(), static_cast<int>(input.size()));
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);

    bool done = false;
    int decodedSize = 0;

    while (!done)
    {
        decodedSize = BIO_read(bio, buffer, static_cast<int>(input.size()));
        if (decodedSize <= 0)
        {
            if (BIO_should_retry(bio))
            {
                continue;
            }
            else
            {
                throw std::runtime_error("Failed to decode Base64 string!");
            }
        }
        else
        {
            done = true;
        }
    }

    std::vector<uint8_t> decoded{};
    decoded.insert(decoded.end(), buffer, buffer + decodedSize);

    delete[] buffer;
    BIO_free_all(bio);

    return decoded;
}

std::string SHA1Hash(const std::vector<uint8_t> &data)
{
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == nullptr)
    {
        throw std::runtime_error("Failed to create EVP context.");
    }

    SCOPE_EXIT
    {
        EVP_MD_CTX_free(mdctx);
    };

    const EVP_MD *sha1 = EVP_sha1();

    if (EVP_DigestInit_ex(mdctx, sha1, nullptr) != 1)
    {
        throw std::runtime_error("EVP_DigestInit_ex failed.");
    }

    if (EVP_DigestUpdate(mdctx, data.data(), data.size()) != 1)
    {
        throw std::runtime_error("EVP_DigestUpdate failed.");
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;

    if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1)
    {
        throw std::runtime_error("EVP_DigestFinal_ex failed.");
    }

    std::ostringstream oss{};
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return oss.str();
}

std::string SHA1Hash(const std::string &data)
{
    return SHA1Hash(std::vector<uint8_t>(data.begin(), data.end()));
}

std::string SHA256Hash(const std::vector<uint8_t> &data)
{
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == nullptr)
    {
        throw std::runtime_error("Failed to create EVP context.");
    }

    SCOPE_EXIT
    {
        EVP_MD_CTX_free(mdctx);
    };

    const EVP_MD *sha256 = EVP_sha256();

    if (EVP_DigestInit_ex(mdctx, sha256, nullptr) != 1)
    {
        throw std::runtime_error("EVP_DigestInit_ex failed.");
    }

    if (EVP_DigestUpdate(mdctx, data.data(), data.size()) != 1)
    {
        throw std::runtime_error("EVP_DigestUpdate failed.");
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;

    if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1)
    {
        throw std::runtime_error("EVP_DigestFinal_ex failed.");
    }

    std::ostringstream oss{};
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return oss.str();
}

std::string SHA256Hash(const std::string &data)
{
    return SHA256Hash(std::vector<uint8_t>(data.begin(), data.end()));
}

std::string SHA256HashFile(const std::string &filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Cannot open file");
    }

    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(fileSize);

    if (!file.read(reinterpret_cast<char *>(buffer.data()), fileSize))
    {
        throw std::runtime_error("Error reading file");
    }

    return SHA256Hash(buffer);
}
} // namespace security