import { Buffer } from 'buffer';
import { NativeModules, Platform } from 'react-native';

const LINKING_ERROR =
  `The package 'react-native-zstd' doesn't seem to be linked. Make sure: \n\n` +
  Platform.select({ ios: "- You have run 'pod install'\n", default: '' }) +
  '- You rebuilt the app after installing the package\n' +
  '- You are not using Expo Go\n';

// @ts-expect-error
const isTurboModuleEnabled = global.__turboModuleProxy != null;

const ZstdModule = isTurboModuleEnabled
  ? require('./NativeZstd').default
  : NativeModules.Zstd;

const _Zstd = ZstdModule
  ? ZstdModule
  : new Proxy(
      {},
      {
        get() {
          throw new Error(LINKING_ERROR);
        },
      }
    );

interface IZstdNative {
  compress(data: string, compressLevel: number): Buffer;
  compressBuffer(data: ArrayBuffer, compressLevel: number): Buffer;
  decompress(data: ArrayBuffer): string;
  decompressBuffer(data: ArrayBuffer): Buffer;
  decompressStreamBuffer(data: ArrayBuffer): Buffer;
}

if ((global as any).__ZSTDProxy == null) {
  _Zstd.install();
}

const ZstdNative: IZstdNative = (global as any).__ZSTDProxy;

export const compress = (data: string, compressLevel: number = 3): Buffer => {
  const out = ZstdNative.compress(data, compressLevel);
  return Buffer.from(out);
};

export const compressBuffer = (
  data: ArrayBuffer,
  compressLevel: number = 3
): Buffer => {
  const out = ZstdNative.compressBuffer(data, compressLevel);
  return Buffer.from(out);
};

export const decompress = (data: Buffer): string => {
  const out = ZstdNative.decompress(data.buffer);
  return out;
};

export const decompressBuffer = (data: Buffer): Buffer => {
  const out = ZstdNative.decompressBuffer(data.buffer);
  return Buffer.from(out);
};

export const decompressStreamBuffer = (data: Buffer): Buffer => {
  const out = ZstdNative.decompressStreamBuffer(data.buffer);
  return Buffer.from(out);
};

const Zstd = {
  compress,
  compressBuffer,
  decompress,
  decompressBuffer,
  decompressStreamBuffer,
};
export default Zstd;
