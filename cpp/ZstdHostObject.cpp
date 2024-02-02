//
// Created by Andarius on 04/09/2022.
// JSI Functions: https://github.com/facebook/react-native/blob/0.68-stable/ReactCommon/jsi/jsi/jsi.h#L647
//

#include "ZstdHostObject.h"

#include "react-native-zstd.h"
#include "jsi-utils/JSIUtils.h"
#include <cstring>


namespace rnzstd {

    namespace jsi = facebook::jsi;


    jsi::Value JSICompress(jsi::Runtime &runtime,
                           jsi::String &buffIn,
                           int compressionLevel) {
        char *_buffIn;
        JSIStringToChar(runtime, buffIn, _buffIn);

        unsigned int compressedSizeOut;
        auto buffOut = compress(_buffIn, compressionLevel, compressedSizeOut);

        delete[] _buffIn;

        auto arr = bytesToJSIArray(runtime, buffOut, compressedSizeOut);

        return {runtime, arr};
    }

    jsi::Value JSICompressBuffer(jsi::Runtime &runtime, 
                                jsi::ArrayBuffer &buffIn, 
                                int compressionLevel) {
        size_t compressedSizeOut;
        const auto buffSize = buffIn.size(runtime);
        const auto _buffIn = buffIn.data(runtime);

        auto buffOut = compressBuffer(_buffIn, buffSize, compressionLevel, compressedSizeOut);
        auto arr = bytesToJSIArray(runtime, buffOut, compressedSizeOut);

        delete[] buffOut;

        return {runtime, arr};
    }

    jsi::Value JSIDecompress(jsi::Runtime &runtime,
                             jsi::ArrayBuffer &buffIn) {
        unsigned int compressedSizeOut;
        const auto buffSize = buffIn.size(runtime);
        const auto _buffIn = buffIn.data(runtime);

        auto buffOut = decompress(_buffIn, buffSize, compressedSizeOut);
        auto str = jsi::String::createFromAscii(runtime, buffOut, compressedSizeOut);

        delete[] buffOut;

        return {runtime, str};
    }


jsi::Value JSIDecompressBuffer(jsi::Runtime &runtime, jsi::ArrayBuffer &buffIn) {
        size_t decompressedSizeOut;
        const auto buffSize = buffIn.size(runtime);
        const auto _buffIn = reinterpret_cast<const uint8_t*>(buffIn.data(runtime));

        auto buffOut = decompressBuffer(_buffIn, buffSize, decompressedSizeOut);
        auto arr = bytesToJSIArray(runtime, buffOut, decompressedSizeOut);

        delete[] buffOut;

        return {runtime, arr};
    }

    ZstdHostObject::ZstdHostObject() {
        this->fields.insert(GET_FIELD("compress",
                                      {
                                          CHECK(count != 2, "compress(...) expects 2 arguments")
                                          auto buffIn = arguments[0].getString(runtime);
                                          auto compressionLevel = arguments[1].getNumber();
                                          return JSICompress(runtime, buffIn, compressionLevel);
                                      }));

        this->fields.insert(GET_FIELD("compressBuffer",
                                      {
                                          CHECK(count != 2, "compressBuffer(...) expects 2 arguments")
                                          jsi::ArrayBuffer buffIn = arguments[0].getObject(runtime).getArrayBuffer(runtime);
                                          auto compressionLevel = arguments[1].getNumber();
                                          return JSICompressBuffer(runtime, buffIn, compressionLevel);
                                      }));

        this->fields.insert(GET_FIELD("decompress", {
            CHECK(count != 1, "decompress(...) expects 1 argument")
            jsi::ArrayBuffer buffIn = arguments[0].getObject(runtime).getArrayBuffer(runtime);
           try {
                return JSIDecompress(runtime, buffIn);
           } catch(const ZstdError &err){
               throw jsi::JSError(runtime, err.what());
           }
        }));

        this->fields.insert(GET_FIELD("decompressBuffer", {
            CHECK(count != 1, "decompressBuffer(...) expects 1 argument")
            jsi::ArrayBuffer buffIn = arguments[0].getObject(runtime).getArrayBuffer(runtime);
           try {
                return JSIDecompressBuffer(runtime, buffIn);
           } catch(const ZstdError &err){
               throw jsi::JSError(runtime, err.what());
           }
        }));
    }


}
