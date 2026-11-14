---
title: Rectangle Sum (長方形和) [rectangle_sum]
documentation_of: ../../../../src/structure/wavelet_matrix/static/rectangle_sum.hpp
---

重み付き点集合に対して、静的に矩形内の重みの総和を求める。`WaveletMatrix2DWeighted` を継承し、コンストラクタもそのまま引き継ぐ。

## テンプレート引数

```cpp
RectangleSum<X, Y, W, MAX_SIZE = -1, Y_BIT_WIDTH = (Y の bit 数)>
```

意味は `WaveletMatrix2DWeighted` と同じである。`MAX_SIZE` を省略すると可変長版になる。

```cpp
using Variable = RectangleSum<X, Y, W>;
using Fixed = RectangleSum<X, Y, W, N>;
```

## コンストラクタ

```cpp
RectangleSum(
    const vector<X>& xs,
    const vector<Y>& ys,
    const vector<W>& weights
)
explicit RectangleSum(const vector<tuple<X, Y, W>>& points)
```

## 矩形内の点数・重みの総和

```cpp
int size() const
int rectangle_count(X xl, X xr, Y yl, Y yr) const
int range_freq(X xl, X xr, Y yl, Y yr) const
W rectangle_sum(X xl, X xr, Y yl, Y yr) const
W range_sum(X xl, X xr, Y yl, Y yr) const
```

`size` は点数を返す。`rectangle_count` / `range_freq` は半開矩形 $[\mathit{xl}, \mathit{xr}) \times [\mathit{yl}, \mathit{yr})$ に含まれる点の個数を、`rectangle_sum` / `range_sum` は重みの総和を返す。

## 時間計算量

$N$ を点数、$H = \texttt{Y_BIT_WIDTH}$ とする。

- コンストラクタ: 可変長版は $O(N \log N + H N)$、固定長版は $O(N \log N + H \cdot \texttt{MAX_SIZE})$
- `size`: $O(1)$
- `rectangle_count`, `range_freq`, `rectangle_sum`, `range_sum`: $O(\log N + H)$

## 注意点

- 入力列の長さが異なる場合、点数が `MAX_SIZE` を超える場合、$y$ 座標が `Y_BIT_WIDTH` bit に収まらない場合、`xl > xr` または `yl > yr` の場合は `std::runtime_error` を送出する。
- コピーは禁止、ムーブは可能。
