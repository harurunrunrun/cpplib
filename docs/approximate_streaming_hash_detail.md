---
title: Streaming Hash Helpers (streaming hash補助)
documentation_of: ../src/approximate/streaming/hash.hpp
---

## Streaming Hash Helpers (streaming hash補助)

streaming sketchが共有する64-bit hashと一様乱数変換の内部API。

## API

### `splitmix64(value)`

64-bit整数をSplitMix64のmixing関数で変換する。時間・追加空間計算量は $O(1)$。

### `key_hash(key, seed)`

`std::hash<Key>` の値にseedを加えて `splitmix64` でmixする。`std::hash` の時間・追加空間計算量を $T_H,S_H$ として時間計算量は $O(T_H)$、追加空間計算量は $O(S_H)$。

### `unit_open(value)`

64-bit値の上位53bitを開区間 $(0,1)$ の `long double` へ写す。時間・追加空間計算量は $O(1)$。

## 注意点

すべて `approximate_streaming_detail` の内部APIである。暗号学的hashではない。`key_hash` は `std::hash<Key>` が利用可能であることを要求し、異なる標準library実装間で同じ値になる保証はない。
