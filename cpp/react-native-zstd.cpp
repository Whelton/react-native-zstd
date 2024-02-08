#include "react-native-zstd.h"
#include "zstd.h"

#include <cstring>
#include <vector>


namespace rnzstd {


    uint8_t *compress(const char *buffIn, int compressionLevel, unsigned int &compressedSizeOut) {
        int const inputSize = (int) strlen(buffIn);

        size_t const outputSize = ZSTD_compressBound(inputSize);
        auto buffOut = new uint8_t[outputSize];

        size_t const compressedSize = ZSTD_compress(buffOut, outputSize, buffIn, inputSize,
                                                    compressionLevel);
        if (ZSTD_isError(compressedSize)) {
            throw ZstdError(ZSTD_getErrorName(compressedSize));
        }
        compressedSizeOut = compressedSize;

        return buffOut;
    }

    uint8_t *compressBuffer(const uint8_t *buffIn, size_t sourceSize, int compressionLevel, size_t &compressedSizeOut) {
        size_t const outputSize = ZSTD_compressBound(sourceSize);
        auto buffOut = new uint8_t[outputSize];

        size_t const compressedSize = ZSTD_compress(buffOut, outputSize, buffIn, sourceSize, 
                                                    compressionLevel);
        if (ZSTD_isError(compressedSize)) {
            throw ZstdError(ZSTD_getErrorName(compressedSize));
        }
        compressedSizeOut = compressedSize;

        return buffOut;
    }

    const char *decompress(const u_int8_t* buffIn,
                           size_t sourceSize,
                           unsigned int &decompressedSizeOut) {
        unsigned long long const outputSize = ZSTD_getFrameContentSize(buffIn, sourceSize);
        if (outputSize == ZSTD_CONTENTSIZE_ERROR) {
            throw ZstdError("Not compressed by zstd");
        }
        if (outputSize == ZSTD_CONTENTSIZE_UNKNOWN) {
            throw ZstdError("Original size unknown");
        }

        auto const buffOut = new char[outputSize];
        size_t const dSize = ZSTD_decompress(buffOut, outputSize, buffIn, sourceSize);
        decompressedSizeOut = dSize;

        /* When zstd knows the content size, it will error if it doesn't match. */
        if (dSize != outputSize) {
            throw ZstdError("Impossible because zstd will check this condition");
        }

        return buffOut;
    }

    uint8_t *decompressBuffer(const uint8_t *buffIn, 
                              size_t sourceSize, 
                              size_t &decompressedSizeOut) {
        unsigned long long const outputSize = ZSTD_getFrameContentSize(buffIn, sourceSize);
        if (outputSize == ZSTD_CONTENTSIZE_ERROR) {
            throw ZstdError("Not compressed by zstd");
        }
        if (outputSize == ZSTD_CONTENTSIZE_UNKNOWN) {
            throw ZstdError("Original size unknown");
        }

        auto buffOut = new uint8_t[outputSize];
        size_t const dSize = ZSTD_decompress(buffOut, outputSize, buffIn, sourceSize);
        decompressedSizeOut = dSize;

        /* When zstd knows the content size, it will error if it doesn't match. */
        if (dSize != outputSize) {
            throw ZstdError("Impossible because zstd will check this condition");
        }

        return buffOut;
    }

    uint8_t *decompressStreamBuffer(const uint8_t *buffIn, size_t sourceSize, size_t &decompressedSizeOut) {
        // References:
        // - https://github.com/google/sandboxed-api/blob/f708270f350ed1ba5c4a751abba6ff2d8e2442eb/contrib/zstd/wrapper/wrapper_zstd.cc#L147

        unsigned long long const outputSize = ZSTD_getFrameContentSize(buffIn, sourceSize);
        if (outputSize == ZSTD_CONTENTSIZE_ERROR) {
            throw ZstdError("Not compressed by zstd");
        }

        ZSTD_DCtx* dctx = ZSTD_createDCtx();
        size_t const sizein = ZSTD_CStreamInSize();
        size_t const sizeout = ZSTD_CStreamOutSize();

        auto bufin = std::make_unique<uint8_t[]>(sizein);
        auto bufout = std::make_unique<uint8_t[]>(sizeout);

        size_t readPos = 0;
        size_t decompressedSize = 0;
        std::vector<uint8_t> decompressedData;

        while (readPos < sourceSize) {
            size_t toRead = std::min(sizein, sourceSize - readPos);
            memcpy(bufin.get(), buffIn + readPos, toRead);
            ZSTD_inBuffer input = { bufin.get(), toRead, 0 };

            while (input.pos < input.size) {
                ZSTD_outBuffer output = { bufout.get(), sizeout, 0 };
                size_t ret = ZSTD_decompressStream(dctx, &output, &input);
                if (ZSTD_isError(ret)) {
                    ZSTD_freeDCtx(dctx);
                    throw ZstdError(ZSTD_getErrorName(ret));
                }
                decompressedData.insert(decompressedData.end(), bufout.get(), bufout.get() + output.pos);
                decompressedSize += output.pos;
            }
            readPos += toRead;
        }

        ZSTD_freeDCtx(dctx);

        auto decompressedBuff = new uint8_t[decompressedSize];
        memcpy(decompressedBuff, decompressedData.data(), decompressedSize);
        decompressedSizeOut = decompressedSize;

        return decompressedBuff;
    }

}
