---
title: Wavelet Matrix 2D (二次元ウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/static/wavelet_matrix_2d.hpp
---

点集合に対する静的な二次元ウェーブレット行列。矩形内の点の個数などを求める。

## テンプレート引数

```cpp
WaveletMatrix2D<X, Y, MAX_SIZE = -1, Y_BIT_WIDTH = (Y の bit 数)>
```

- `X`: $x$ 座標の整数型
- `Y`: $y$ 座標の整数型
- `MAX_SIZE`: 点数の上限。省略時（`-1`）は `std::vector` を使う可変長版になり、入力長に応じた領域だけを確保する。非負の値を指定すると固定長配列版になる。
- `Y_BIT_WIDTH`: $y$ 座標に使う bit 数。省略時は `Y` の bit 数。`Y` が符号付きの場合は省略しなければならない（`static_assert`）。

```cpp
using Variable = WaveletMatrix2D<X, Y>;
using Fixed = WaveletMatrix2D<X, Y, N>;
```

## コンストラクタ

```cpp
WaveletMatrix2D(const vector<X>& xs, const vector<Y>& ys)
explicit WaveletMatrix2D(const vector<pair<X, Y>>& points)
int size() const
```

点 $(\mathit{xs}_i, \mathit{ys}_i)$ の集合、または `points` から構築する。内部で点を $x$ 座標の順に並べるため、入力の順序はクエリ結果に影響しない。`size` は点数を返す。

## 矩形内の点数

```cpp
int rectangle_count(X xl, X xr, Y yl, Y yr) const
int range_freq(X xl, X xr, Y yl, Y yr) const
```

半開矩形 $[\mathit{xl}, \mathit{xr}) \times [\mathit{yl}, \mathit{yr})$ に含まれる点の個数を返す。2 つは同じ関数である。

## x 範囲内の y 座標

```cpp
Y kth_smallest_y(X xl, X xr, int k) const
optional<Y> prev_y(X xl, X xr, Y upper) const
optional<Y> next_y(X xl, X xr, Y lower) const
```

$x$ 座標が $[\mathit{xl}, \mathit{xr})$ に入る点の $y$ 座標について、`kth_smallest_y` は `k` 番目（0-indexed）に小さい値を、`prev_y` は `upper` 未満の最大値を、`next_y` は `lower` 以上の最小値を返す。`prev_y` / `next_y` は存在しなければ `nullopt` を返す。

## 時間計算量

$N$ を点数、$H = \texttt{Y_BIT_WIDTH}$ とし、$W$ を固定長版では $\lceil \texttt{MAX_SIZE} / 64 \rceil$、可変長版では $\lceil N / 64 \rceil$ とする。

- コンストラクタ: $O(N \log N + H(N + W))$（固定長版では配列の初期化に $O(\texttt{MAX_SIZE})$ が加わる）
- `size`: $O(1)$
- `rectangle_count`, `range_freq`, `kth_smallest_y`, `prev_y`, `next_y`: $O(\log N + H)$

## 注意点

- 2 つの入力列の長さが異なる場合、点数が `MAX_SIZE` を超える場合、$y$ 座標が `Y_BIT_WIDTH` bit に収まらない場合、`xl > xr` または `yl > yr` の場合、`k` が範囲外の場合は `std::runtime_error` を送出する。
- コピーは禁止、ムーブは可能。
