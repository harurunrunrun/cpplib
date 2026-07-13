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

# 共通API契約

点id `k` は入力順を保つ。`size` は点数、`x(k)` と `y(k)` は現在座標を返す。
x座標は構築後固定、`set_y(k,y)` はyだけを更新する。`rectangle_count/range_freq` は
半開矩形 `[xl,xr) x [yl,yr)` の点数。`kth_smallest_y` は0-indexed、
`prev_y/next_y` は該当なしなら `nullopt`。

vector長不一致、点数容量、yのbit幅、無効な点・矩形・順序では `runtime_error`。
copyは禁止、moveは可能。各APIの計算量は上記表の通り。
