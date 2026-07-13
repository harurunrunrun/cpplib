---
title: Rectangle Sum
documentation_of: ../src/structure/wavelet_matrix/rectangle_sum.hpp
---

重み付き点集合に対して、静的な矩形和を求める。

# テンプレート引数

```cpp
RectangleSum<X, Y, W, MAX_SIZE, Y_BIT_WIDTH>
```

`WaveletMatrix2DWeighted` と同じ。

# コンストラクタ

```cpp
RectangleSum(
    const vector<X>& xs,
    const vector<Y>& ys,
    const vector<W>& weights
)
RectangleSum(const vector<tuple<X, Y, W>>& points)
```

# 矩形内の重みの総和

```cpp
W rectangle_sum(X xl, X xr, Y yl, Y yr) const
W range_sum(X xl, X xr, Y yl, Y yr) const
```

半開矩形 $[xl,xr)\times[yl,yr)$ に含まれる点の重みの総和を返す。

## 時間計算量

- 構築: $O(N\log N+\mathrm{Y\_BIT\_WIDTH}\cdot\mathtt{MAX\_SIZE})$
- クエリ: $O(\log N + \mathrm{Y\_BIT\_WIDTH})$

# 計算量（公開操作別）

$H=\mathtt{Y\_BIT\_WIDTH}$ とする。

- 2種類のconstructor: $O(N\log N+H\mathtt{MAX\_SIZE})$
- `size`: $O(1)$
- `rectangle_count`, `range_freq`, `rectangle_sum`, `range_sum`: $O(\log N+H)$
