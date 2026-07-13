---
title: Linear Counting (線形カウンティング)
documentation_of: ../src/approximate/streaming/linear_counting.hpp
---

# Linear Counting (線形カウンティング)

distinct key を bit map に写し、空 bit 数から cardinality を推定する。

## API

- `LinearCounting(seed)`: 空で構築する。時間計算量は $O(BitCount/64)$。 追加空間計算量は $O(1)$。
- `add(key)`: hash 先の bit を立てる。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `estimate()`: $-m\log(V/m)$ を返す。空 bit がなければ無限大。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `merge(other)`: bit ごとの OR を取る。時間計算量は $O(BitCount/64)$。 追加空間計算量は $O(1)$。
- `clear()`: bit map を消す。時間計算量は $O(BitCount/64)$。 追加空間計算量は $O(1)$。
- `seed()`: hash seed を返す。時間計算量は $O(1)$。 追加空間計算量は $O(1)$。
- `occupied_bits()`, `bit_count()`: 時間計算量は $O(1)$。 追加空間計算量は $O(1)$。


## 注意点

異なる seed の merge は `std::invalid_argument`。一様 hash の下で maximum likelihood estimator
となる。負荷 $n/m$ が小さい範囲では誤差は $O(1/\sqrt m)$、bit map が飽和すると
分散が急増する。保存に必要な空間計算量は $O(BitCount)$ bit。
