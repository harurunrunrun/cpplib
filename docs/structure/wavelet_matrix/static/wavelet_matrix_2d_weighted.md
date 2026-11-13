---
title: Weighted Wavelet Matrix 2D (二次元重み付きウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/static/wavelet_matrix_2d_weighted.hpp
---

重み付き点集合に対する静的な二次元ウェーブレット行列。矩形内の点の個数と重みの総和を求める。

## テンプレート引数

```cpp
WaveletMatrix2DWeighted<X, Y, W, MAX_SIZE = -1, Y_BIT_WIDTH = (Y の bit 数)>
```

- `X`: $x$ 座標の整数型
- `Y`: $y$ 座標の整数型
- `W`: 重みの型。加算・減算と値初期化 `W{}`（零元）が必要
- `MAX_SIZE`: 点数の上限。省略時（`-1`）は `std::vector` を使う可変長版になり、入力長に応じた領域だけを確保する。非負の値を指定すると固定長配列版になる。
- `Y_BIT_WIDTH`: $y$ 座標に使う bit 数。省略時は `Y` の bit 数。`Y` が符号付きの場合は省略しなければならない（`static_assert`）。

```cpp
using Variable = WaveletMatrix2DWeighted<X, Y, W>;
using Fixed = WaveletMatrix2DWeighted<X, Y, W, N>;
```

## コンストラクタ

```cpp
WaveletMatrix2DWeighted(
    const vector<X>& xs,
    const vector<Y>& ys,
    const vector<W>& weights
)
explicit WaveletMatrix2DWeighted(const vector<tuple<X, Y, W>>& points)
int size() const
```

座標 $(\mathit{xs}_i, \mathit{ys}_i)$、重み $\mathit{weights}_i$ の点の集合、または `points` から構築する。3 つの列の長さは一致しなければならない。内部で点を $x$ 座標の順に並べるため、入力の順序はクエリ結果に影響しない。`size` は点数を返す。

## 矩形内の点数

```cpp
int rectangle_count(X xl, X xr, Y yl, Y yr) const
int range_freq(X xl, X xr, Y yl, Y yr) const
```

半開矩形 $[\mathit{xl}, \mathit{xr}) \times [\mathit{yl}, \mathit{yr})$ に含まれる点の個数を返す。2 つは同じ関数である。

## 矩形内の重みの総和

```cpp
W rectangle_sum(X xl, X xr, Y yl, Y yr) const
W range_sum(X xl, X xr, Y yl, Y yr) const
```

半開矩形 $[\mathit{xl}, \mathit{xr}) \times [\mathit{yl}, \mathit{yr})$ に含まれる点の重みの総和を返す。2 つは同じ関数である。

## 時間計算量

$N$ を点数、$H = \texttt{Y_BIT_WIDTH}$ とする。

- コンストラクタ: 可変長版は $O(N \log N + H N)$、固定長版は配列の初期化を含めて $O(N \log N + H \cdot \texttt{MAX_SIZE})$
- `size`: $O(1)$
- `rectangle_count`, `range_freq`, `rectangle_sum`, `range_sum`: $O(\log N + H)$

## 空間計算量

各段に重みの累積和を持つため $O(H N)$（固定長版では $O(H \cdot \texttt{MAX_SIZE})$）。

## 注意点

- 入力列の長さが異なる場合、点数が `MAX_SIZE` を超える場合、$y$ 座標が `Y_BIT_WIDTH` bit に収まらない場合、`xl > xr` または `yl > yr` の場合は `std::runtime_error` を送出する。
- コピーは禁止、ムーブは可能。
