---
title: SimHash (類似度ハッシュ)
documentation_of: ../src/approximate/streaming/simhash.hpp
---

重み付き feature vector の cosine 類似度を bit signature に圧縮する。

- `SimHash(seed)`: accumulator を 0 にする。$O(Bits)$。
- `add(feature, weight)`: feature の符号 vector を加算する。$O(Bits)$。
- `signature()`: accumulator の符号を bit 列にする。$O(Bits)$。
- `similarity(other)`: $1-$正規化 Hamming 距離。$O(Bits)$。
- `hamming_distance(a, b)`: Hamming 距離。$O(1)$。
- `clear()`: accumulator を 0 にする。$O(Bits)$。
- `seed()`: hash seed を返す。$O(1)$。
- `bit_count()`: $O(1)$。

異なる seed の `similarity` は `std::invalid_argument`。
ランダム超平面モデルでは角度を $\theta$ とした 1 bit の一致確率は $1-\theta/\pi$。
独立 bit なら推定値の標準偏差は高々 $1/(2\sqrt{Bits})$。保存領域は $O(Bits)$。
