---
title: Partially Persistent Weighted Wavelet Matrix 2D (部分永続二次元重み付きウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/partially_persistent/partially_persistent_wavelet_matrix_2d_weighted.hpp
---

入力順の点 ID と $x$ 座標を固定し、最新版の $y$ 座標と重みを更新しながら過去の版を参照できる部分永続の重み付き二次元ウェーブレット行列。完全永続版をラップせず、最新版のみを更新する `PartiallyPersistentWeightedWaveletMatrix` を使う。

## テンプレート引数

```cpp
PartiallyPersistentWaveletMatrix2DWeighted<X, Y, W, MAX_SIZE = -1, MAX_VERSION = -1, Y_BIT_WIDTH = (Y の bit 数)>
```

- `X`, `Y`: 整数型
- `W`: 重みの型。`W{}` を零元として加算・減算が必要
- `MAX_SIZE`: 点数の上限
- `MAX_VERSION`: 版 0 の後に実行できる更新の回数の上限
- `Y_BIT_WIDTH`: $y$ 座標を表す bit 数。省略時は `Y` の bit 数。符号付きの `Y` では省略しなければならない。

`MAX_SIZE` と `MAX_VERSION` は省略（`-1`）すると上限なしになる。内部の永続ウェーブレット行列は必要に応じて伸長するが、`MAX_SIZE` を指定すると $x$ 座標と添字の対応表に $O(\texttt{MAX_SIZE})$ の固定長配列を確保する。

```cpp
using Variable = PartiallyPersistentWaveletMatrix2DWeighted<X, Y, W>;
using Fixed = PartiallyPersistentWaveletMatrix2DWeighted<X, Y, W, N, Versions>;
```

## API

```cpp
PartiallyPersistentWaveletMatrix2DWeighted()
PartiallyPersistentWaveletMatrix2DWeighted(const vector<X>& xs, const vector<Y>& ys, const vector<W>& weights)
explicit PartiallyPersistentWaveletMatrix2DWeighted(const vector<tuple<X, Y, W>>& points)
int size() const
int versions() const
int latest_version() const
X x(int k) const
Y y(int version, int k) const
W weight(int version, int k) const
int set(int k, Y y, W weight)
int set_y(int k, Y y)
int set_weight(int k, W weight)
int rectangle_count(int version, X xl, X xr, Y yl, Y yr) const
W rectangle_sum(int version, X xl, X xr, Y yl, Y yr) const
int range_freq(int version, X xl, X xr, Y yl, Y yr) const
W range_sum(int version, X xl, X xr, Y yl, Y yr) const
```

- コンストラクタ: 3 本の列、または `tuple<X, Y, W>` の列の `k` 番目を点 ID `k` として版 0 を構築する。
- `x` / `y` / `weight`: 点 `k` の $x$ 座標（全版で不変）/ 版 `version` での $y$ 座標 / 重みを返す。
- `set` / `set_y` / `set_weight`: 最新版をもとに点 `k` の $y$ 座標と重み / $y$ 座標だけ / 重みだけを更新した新しい版を作り、その番号を返す。
- `rectangle_count` / `rectangle_sum`: 版 `version` で半開矩形 $[\mathit{xl}, \mathit{xr}) \times [\mathit{yl}, \mathit{yr})$ に含まれる点の個数 / 重みの総和を返す。`range_freq` / `range_sum` はそれぞれの別名である。

## 時間計算量

$N$ を点数、$D = \texttt{Y_BIT_WIDTH}$、$H = O(\log(N + 1))$ とする。最新版と過去の版でクエリの計算量は同じである。

- 構築: $O(N \log N + DN)$（固定長版では配列の初期化に $O(\texttt{MAX_SIZE})$ が加わる）
- `size`, `versions`, `latest_version`, `x`: $O(1)$
- `weight`: $O(H)$、`y`: $O(DH)$
- `set`, `set_y`, `set_weight`: $O(DH)$
- 矩形の個数・総和とその別名: $O(\log N + DH)$

更新 1 回あたりの追加メモリは $O(DH)$ である。

## 注意点

- 更新元は最新版だけである。同じ $x$ 座標の点も入力時の ID で区別する。
- 不正な版・点・矩形、入力列の長さの不一致、`Y_BIT_WIDTH` bit に収まらない $y$ 座標、容量超過では `std::runtime_error` を送出する。失敗した更新は、版の数と全 bit 段のノード使用量を元に戻す。
- コピーは禁止、ムーブは可能。
