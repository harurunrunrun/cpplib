---
title: Dynamic Weighted Wavelet Matrix 2D
documentation_of: ../src/structure/wavelet_matrix/dynamic_wavelet_matrix_2d_weighted.hpp
---

x座標を固定し、y座標と重みを点更新できる2次元wavelet matrix。

# 更新

```cpp
wm.set(k, y, weight)
wm.set_y(k, y)
wm.set_weight(k, weight)
```

# クエリ

```cpp
wm.rectangle_count(xl, xr, yl, yr)
wm.rectangle_sum(xl, xr, yl, yr)
wm.range_freq(xl, xr, yl, yr)
wm.range_sum(xl, xr, yl, yr)
```

# 時間計算量

`DynamicWeightedWaveletMatrix` のクエリに、x範囲の二分探索が加わる。

# 計算量（公開操作別）

$B=\mathtt{BLOCK\_SIZE}$ とし、x範囲に入る点数を $L$、
$Q(L)=B+\lceil L/B\rceil\log B$ とする。

- default constructor: $O(1)$
- vector/points constructor: $O(\mathtt{MAX\_SIZE}+N(\log N+\log B))$
- `size`, `x`, `y`, `weight`: $O(1)$
- `set`, `set_y`, `set_weight`: $O(B\log B)$
- `rectangle_count`, `range_freq`, `rectangle_sum`, `range_sum`: $O(\log N+Q(L))$
