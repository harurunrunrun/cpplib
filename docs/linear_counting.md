---
title: Linear Counting (線形カウンティング)
documentation_of: ../src/approximate/streaming/linear_counting.hpp
---

distinct key を bit map に写し、空 bit 数から cardinality を推定する。

- `LinearCounting(seed)`: 空で構築する。$O(BitCount/64)$。
- `add(key)`: hash 先の bit を立てる。$O(1)$。
- `estimate()`: $-m\log(V/m)$ を返す。空 bit がなければ無限大。$O(1)$。
- `merge(other)`: bit ごとの OR を取る。$O(BitCount/64)$。
- `clear()`: bit map を消す。$O(BitCount/64)$。
- `seed()`: hash seed を返す。$O(1)$。
- `occupied_bits()`, `bit_count()`: $O(1)$。

異なる seed の merge は `std::invalid_argument`。一様 hash の下で maximum likelihood estimator
となる。負荷 $n/m$ が小さい範囲では誤差は $O(1/\sqrt m)$、bit map が飽和すると
分散が急増する。保存領域は $O(BitCount)$ bit。
