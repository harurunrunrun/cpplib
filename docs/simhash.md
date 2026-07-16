---
title: SimHash (類似度ハッシュ)
documentation_of: ../src/approximate/streaming/simhash.hpp
---

## SimHash (類似度ハッシュ)

重み付き feature vector の cosine 類似度を bit signature に圧縮する。

## API

- `SimHash(seed)`: accumulator を 0 にする。時間計算量は $O(Bits)$。 追加空間計算量は $O(1)$。
- `add(feature, weight)`: feature の符号 vector を加算する。時間計算量は $O(Bits)$。 追加空間計算量は $O(1)$。
- `signature()`: accumulator の符号を bit 列にする。時間計算量は $O(Bits)$。 追加空間計算量は $O(1)$。
- `similarity(other)`: $1-$正規化 Hamming 距離。時間計算量は $O(Bits)$。 追加空間計算量は $O(1)$。
- `hamming_distance(a, b)`: Hamming 距離。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `clear()`: accumulator を 0 にする。時間計算量は $O(Bits)$。 追加空間計算量は $O(1)$。
- `seed()`: hash seed を返す。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `bit_count()`: 時間計算量は $O(1)$。 追加空間計算量は $O(1)$。


## 注意点

異なる seed の `similarity` は `std::invalid_argument`。
ランダム超平面モデルでは角度を $\theta$ とした 1 bit の一致確率は $1-\theta/\pi$。
独立 bit なら推定値の標準偏差は高々 $1/(2\sqrt{Bits})$。保存に必要な空間計算量は $O(Bits)$。
