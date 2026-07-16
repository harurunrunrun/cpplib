---
title: Weighted Wavelet Matrix 2D (二次元重み付きウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/wavelet_matrix_2d_weighted.hpp
---

重み付き点集合に対する静的2次元wavelet matrix。矩形内の点数と重みの総和を求める。

## テンプレート引数

```cpp
WaveletMatrix2DWeighted<X, Y, W, MAX_SIZE, Y_BIT_WIDTH>
```

- x座標の整数型 `X`
- y座標の整数型 `Y`
- 重みの型 `W`
- 点数の上限 `MAX_SIZE`
- y座標に使用するbit数 `Y_BIT_WIDTH`。省略時は `Y` のbit数

## コンストラクタ

```cpp
WaveletMatrix2DWeighted(
    const vector<X>& xs,
    const vector<Y>& ys,
    const vector<W>& weights
)
WaveletMatrix2DWeighted(const vector<tuple<X, Y, W>>& points)
```

点をx座標でソートして構築する。

## 時間計算量

- $O(N\log N+\mathrm{Y\_BIT\_WIDTH}\cdot\mathtt{MAX\_SIZE})$

## 矩形内の点数

```cpp
int rectangle_count(X xl, X xr, Y yl, Y yr) const
int range_freq(X xl, X xr, Y yl, Y yr) const
```

半開矩形 $[xl,xr)\times[yl,yr)$ に含まれる点数を返す。

## 時間計算量

- $O(\log N + \mathrm{Y\_BIT\_WIDTH})$

## 矩形内の重みの総和

```cpp
W rectangle_sum(X xl, X xr, Y yl, Y yr) const
W range_sum(X xl, X xr, Y yl, Y yr) const
```

半開矩形 $[xl,xr)\times[yl,yr)$ に含まれる点の重みの総和を返す。

## 時間計算量

- $O(\log N + \mathrm{Y\_BIT\_WIDTH})$

## 計算量（公開操作別）

$H=\mathtt{Y\_BIT\_WIDTH}$ とする。

- 2種類のconstructor: $O(N\log N+H\mathtt{MAX\_SIZE})$
- `size`: $O(1)$
- `rectangle_count`, `range_freq`, `rectangle_sum`, `range_sum`: $O(\log N+H)$

## 注意点

constructorは `xs,ys,weights` またはtuple列を受け、内部でx順に並べる。3vectorの長さは一致が必要。
`size` は点数。`rectangle_count/range_freq` は半開矩形
`[xl,xr) x [yl,yr)` の点数、`rectangle_sum/range_sum` は重み総和を返す。

vector長不一致、点数容量、yのbit幅、逆転した座標範囲では `runtime_error`。
copyは禁止、moveは可能。各APIの計算量は上記表の通り。
