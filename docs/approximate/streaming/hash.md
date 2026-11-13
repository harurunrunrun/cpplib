---
title: Streaming Hash Helpers (streaming hash補助)
documentation_of: ../../../src/approximate/streaming/hash.hpp
---

ストリーミング系のスケッチが共有する、64 bit ハッシュと一様乱数への変換の内部 API である。

## API

### `splitmix64(value)`

64 bit 整数を SplitMix64 の混合関数で変換する。

### `key_hash(key, seed)`

`std::hash<Key>` の値にシードを加え、`splitmix64` で混合する。

### `unit_open(value)`

64 bit 値の上位 53 bit を開区間 $(0,1)$ の `long double` へ写す。

## 時間計算量

- `splitmix64`, `unit_open`: 時間・追加空間計算量 $O(1)$
- `key_hash`: `std::hash` の時間・追加空間計算量を $T_H$, $S_H$ として、時間計算量 $O(T_H)$、追加空間計算量 $O(S_H)$

## 注意点

- すべて名前空間 `approximate_streaming_detail` の内部 API である。
- 暗号学的ハッシュではない。
- `key_hash` は `std::hash<Key>` が利用可能であることを要求し、標準ライブラリの実装が異なると同じ値になる保証はない。
