---
title: Dynamic Wavelet Matrix 2D
documentation_of: ../src/structure/wavelet_matrix/dynamic_wavelet_matrix_2d.hpp
---

x座標を固定し、y座標を点更新できる2次元wavelet matrix。

# コンストラクタ

```cpp
DynamicWaveletMatrix2D(xs, ys)
DynamicWaveletMatrix2D(points)
```

# 更新

```cpp
wm.set_y(k, y)
```

元の点番号 `k` のy座標を更新する。x座標は変えない。

# クエリ

```cpp
wm.rectangle_count(xl, xr, yl, yr)
wm.range_freq(xl, xr, yl, yr)
wm.kth_smallest_y(xl, xr, k)
wm.prev_y(xl, xr, upper)
wm.next_y(xl, xr, lower)
```

# 時間計算量

`DynamicWaveletMatrix` のクエリに、x範囲の二分探索が加わる。

# 計算量（公開操作別）

$B=\mathtt{BLOCK\_SIZE}$、$D=\mathtt{Y\_BIT\_WIDTH}$ とし、x範囲に入る点数を $L$、
$Q(L)=B+\lceil L/B\rceil\log B$ とする。

- default constructor: $O(1)$
- vector/points constructor: $O(\mathtt{MAX\_SIZE}+N(\log N+\log B))$
- `size`, `x`, `y`: $O(1)$
- `set_y`: $O(B\log B)$
- `rectangle_count`, `range_freq`: $O(\log N+Q(L))$
- `kth_smallest_y`, `prev_y`, `next_y`: $O(\log N+DQ(L))$
