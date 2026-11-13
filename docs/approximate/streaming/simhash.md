---
title: SimHash (類似度ハッシュ)
documentation_of: ../../../src/approximate/streaming/simhash.hpp
---

重み付きの特徴ベクトルを、`Bits` ビットのシグネチャに圧縮する。シグネチャ間の Hamming 距離から cosine 類似度（角度）を推定できる。テンプレート引数は `SimHash<Bits = 64, Key = std::uint64_t>` で、`Bits` は 1 以上 64 以下。

## API

- `SimHash(seed = 0)`: 累積値を 0 にして構築する。
- `add(feature, weight = 1.0L)`: 特徴 `feature` のハッシュの各ビットに応じて、各成分の累積値に $\pm w$ を加える。
- `signature()`: 累積値が 0 以上の成分を 1 とするビット列（`std::uint64_t`）を返す。
- `similarity(other)`: $1-(\text{Hamming 距離})/\texttt{Bits}$ を返す。
- `hamming_distance(a, b)`（静的）: 下位 `Bits` ビットの Hamming 距離を返す。
- `clear()`: 累積値を 0 にする。
- `seed()`: シードを返す。
- `bit_count()`: `Bits` を返す。

## 時間計算量

- コンストラクタ・`add`・`signature`・`similarity`・`clear`: $O(\texttt{Bits})$
- `hamming_distance`, `seed`, `bit_count`: $O(1)$
- 空間計算量: $O(\texttt{Bits})$

## 注意点

- シードが異なるもの同士の `similarity` は `std::invalid_argument` を投げる。
- ランダム超平面モデルでは、2 つのベクトルのなす角を $\theta$ として、1 ビットが一致する確率は $1-\theta/\pi$ である。ビットが独立なら推定値の標準偏差は高々 $1/(2\sqrt{\texttt{Bits}})$。
