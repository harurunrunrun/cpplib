---
title: Partially Persistent Wavelet Matrix 2D (部分永続二次元ウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/partially_persistent/partially_persistent_wavelet_matrix_2d.hpp
---

入力点の $x$ 座標を固定し、最新版の $y$ 座標だけを点更新できる部分永続二次元ウェーブレット行列。任意の過去の版を矩形・順序クエリで参照できる。点 ID は入力順のまま保持し、内部では $x$ 座標順に並べた点列を `PartiallyPersistentWaveletMatrix` で管理する。

## テンプレート引数

```cpp
PartiallyPersistentWaveletMatrix2D<X, Y, MAX_SIZE = -1, MAX_VERSION = -1, Y_BIT_WIDTH = (Y の bit 数)>
```

- `X`, `Y`: `bool` 以外の整数型
- `MAX_SIZE`: 点数の上限
- `MAX_VERSION`: 版 0 の後に実行できる更新の回数の上限
- `Y_BIT_WIDTH`: $y$ 座標を表す bit 数。省略時は `Y` の bit 数。符号付きの `Y` では省略しなければならない。

`MAX_SIZE` と `MAX_VERSION` は省略（`-1`）すると上限なしになる。内部の永続ウェーブレット行列は必要に応じて伸長するが、`MAX_SIZE` を指定すると $x$ 座標と添字の対応表に $O(\texttt{MAX_SIZE})$ の固定長配列を確保する。

```cpp
using Variable = PartiallyPersistentWaveletMatrix2D<X, Y>;
using Fixed = PartiallyPersistentWaveletMatrix2D<X, Y, N, Versions>;
```

## コンストラクタ

```cpp
PartiallyPersistentWaveletMatrix2D()
PartiallyPersistentWaveletMatrix2D(const vector<X>& xs, const vector<Y>& ys)
explicit PartiallyPersistentWaveletMatrix2D(const vector<pair<X, Y>>& points)
```

`xs[k]` と `ys[k]`、または `points[k]` を点 ID `k` として版 0 を構築する。

## 版・座標・更新

```cpp
int size() const
int versions() const
int latest_version() const
X x(int k) const
Y y(int version, int k) const
int set_y(int k, Y y)
```

`x` は点 `k` の $x$ 座標（全版で不変）を、`y` は版 `version` での点 `k` の $y$ 座標を返す。`set_y` は最新版の点 `k` の $y$ 座標だけを更新した新しい版を作り、その番号を返す。過去の版は読み取り専用である。

## 矩形・順序クエリ

```cpp
int rectangle_count(int version, X xl, X xr, Y yl, Y yr) const
int range_freq(int version, X xl, X xr, Y yl, Y yr) const
Y kth_smallest_y(int version, X xl, X xr, int k) const
optional<Y> prev_y(int version, X xl, X xr, Y upper) const
optional<Y> next_y(int version, X xl, X xr, Y lower) const
```

- `rectangle_count`, `range_freq`: $[\mathit{xl}, \mathit{xr}) \times [\mathit{yl}, \mathit{yr})$ に含まれる点の個数を返す。
- `kth_smallest_y`: $x$ 座標が $[\mathit{xl}, \mathit{xr})$ に入る点の $y$ 座標のうち、`k` 番目（0-indexed）に小さい値を返す。
- `prev_y`: 対象の $x$ 範囲で `upper` 未満の最大の $y$ 座標を返す。存在しなければ `nullopt` を返す。
- `next_y`: 対象の $x$ 範囲で `lower` 以上の最小の $y$ 座標を返す。存在しなければ `nullopt` を返す。

## 時間計算量

$N$ を点数、$D = \texttt{Y_BIT_WIDTH}$ とする。

- コンストラクタ: $O(N \log N + DN)$（固定長版では配列の初期化に $O(\texttt{MAX_SIZE})$ が加わる）
- `size`, `versions`, `latest_version`, `x`: $O(1)$
- `y`, `set_y`: $O(D \log(N + 1))$
- `rectangle_count`, `range_freq`, `kth_smallest_y`, `prev_y`, `next_y`: $O(\log(N + 1) + D \log(N + 1))$

## 空間計算量

版の数を $V$、`set_y` を実行した回数を $U$ とすると、全メモリは $O(DN + UD \log(N + 1) + VD + N)$ である。

## 注意点

- 矩形は半開区間である。点 ID と順序統計は 0-indexed である。
- 更新は常に最新版に対して行う。クエリの `version` には作成済みの任意の版を指定でき、その内容は後の更新で変化しない。
- 不正な版・点・矩形・`k`、入力列の長さの不一致、点数・版数の上限超過、`Y_BIT_WIDTH` bit に収まらない $y$ 座標では `std::runtime_error` を送出する。失敗した更新は新しい版を作らない。
- コピーは禁止、ムーブは可能。
