---
title: Wavelet Matrix 2D
documentation_of: ../src/structure/wavelet_matrix/wavelet_matrix_2d.hpp
---

点集合に対する静的2次元wavelet matrix。矩形内の点数を求める。

# テンプレート引数

```cpp
WaveletMatrix2D<X, Y, MAX_SIZE, Y_BIT_WIDTH>
```

- x座標の整数型 `X`
- y座標の整数型 `Y`
- 点数の上限 `MAX_SIZE`
- y座標に使用するbit数 `Y_BIT_WIDTH`。省略時は `Y` のbit数

# コンストラクタ

```cpp
WaveletMatrix2D(const vector<X>& xs, const vector<Y>& ys)
WaveletMatrix2D(const vector<pair<X, Y>>& points)
```

点をx座標でソートして構築する。

## 時間計算量

- $O(N\log N+\mathtt{MAX\_SIZE}+\mathrm{Y\_BIT\_WIDTH}(N+\lceil\mathtt{MAX\_SIZE}/64\rceil))$

# 矩形内の点数

```cpp
int rectangle_count(X xl, X xr, Y yl, Y yr) const
int range_freq(X xl, X xr, Y yl, Y yr) const
```

半開矩形 $[xl,xr)\times[yl,yr)$ に含まれる点数を返す。

## 時間計算量

- $O(\log N + \mathrm{Y\_BIT\_WIDTH})$

# x範囲内のy座標

```cpp
Y kth_smallest_y(X xl, X xr, int k) const
optional<Y> prev_y(X xl, X xr, Y upper) const
optional<Y> next_y(X xl, X xr, Y lower) const
```

`kth_smallest_y` はx座標が $[xl,xr)$ に入る点のy座標のうち、`k` 番目に小さい値を返す。`k` は0-indexed。

## 時間計算量

- $O(\log N + \mathrm{Y\_BIT\_WIDTH})$

# 計算量（公開操作別）

$H=\mathtt{Y\_BIT\_WIDTH}$、$W=\lceil\mathtt{MAX\_SIZE}/64\rceil$ とする。

- 2種類のconstructor: $O(N\log N+\mathtt{MAX\_SIZE}+H(N+W))$
- `size`: $O(1)$
- `rectangle_count`, `range_freq`, `kth_smallest_y`, `prev_y`, `next_y`: $O(\log N+H)$
