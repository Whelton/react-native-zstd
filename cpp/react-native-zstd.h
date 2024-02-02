#ifndef RNZSTD_H
#define RNZSTD_H

#include <string>
#include <stdexcept>

namespace rnzstd {
    uint8_t *compress(const char *buffIn,
                                 int compressionLevel,
                                 unsigned int &compressedSizeOut);
    uint8_t *compressBuffer(const uint8_t *buffIn, 
                                  size_t sourceSize, 
                                  int compressionLevel, 
                                  size_t &compressedSizeOut);
    const char *decompress(const uint8_t *buffIn,
                                      size_t sourceSize,
                                      unsigned int &decompressedSizeOut);
    uint8_t *decompressBuffer(const uint8_t *buffIn, 
                                      size_t sourceSize, 
                                      size_t &decompressedSizeOut);

    class ZstdError : public std::runtime_error {
    public:
        explicit ZstdError(const std::string &msg) : std::runtime_error{msg} {}
    };

}

#endif /* RNZSTD_H */
