import type { TurboModule } from 'react-native';
import { TurboModuleRegistry } from 'react-native';

export interface Spec extends TurboModule {
  compress(data: string, compressLevel: number): Buffer;
  compressBuffer(data: ArrayBuffer, compressLevel: number): Buffer;
  decompress(data: ArrayBuffer): string;
  decompressBuffer(data: ArrayBuffer): Buffer;
  decompressStreamBuffer(data: ArrayBuffer): Buffer;
}

export default TurboModuleRegistry.getEnforcing<Spec>('Zstd');
