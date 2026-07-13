---
title: Dynamic Weighted Wavelet Matrix 2D (動的二次元重み付きウェーブレット行列)
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

## 注意点

default constructor直後は未構築状態であり、`size()` 以外を呼ばない。vector/points constructorで構築するか、構築済みobjectをmove代入してから利用する。

点idは入力順を保つ。`size` は点数、`x/y/weight` は1点を返し、xは構築後固定。
`set(k,y,w)` はyと重み、`set_y/set_weight` は片方を更新する。
`rectangle_count/range_freq` は半開矩形の点数、`rectangle_sum/range_sum` は重み総和。

vector長不一致、点数容量、yのbit幅、無効な点・矩形では `runtime_error`。
copyは禁止、moveは可能。各APIの計算量は上記表の通り。

# Constructor signature

```cpp
DynamicWaveletMatrix2DWeighted()
DynamicWaveletMatrix2DWeighted(const vector<X>& xs, const vector<Y>& ys, const vector<W>& weights)
DynamicWaveletMatrix2DWeighted(const vector<tuple<X,Y,W>>& points)
```
