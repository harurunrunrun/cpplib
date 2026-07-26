---
title: Persistent Wavelet Matrix 2D (完全永続二次元ウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/persistent/persistent_wavelet_matrix_2d.hpp
---

入力点の x 座標を固定し、y 座標を完全永続に点更新する二次元 Wavelet Matrix。
点 id は入力順のまま保持し、x 座標で安定した検索順を内部に構築する。

## テンプレート引数

```cpp
PersistentWaveletMatrix2D<X, Y, MAX_SIZE, MAX_VERSION, Y_BIT_WIDTH>
```

`X`, `Y` は `bool` 以外の整数型。`MAX_SIZE` は点数、`MAX_VERSION` は version 作成回数の
上限。`Y_BIT_WIDTH` は y 座標を表す bit 数で、省略時は `Y` の全 bit 数である。

## コンストラクタ

```cpp
PersistentWaveletMatrix2D()
PersistentWaveletMatrix2D(const vector<X>& xs, const vector<Y>& ys)
PersistentWaveletMatrix2D(const vector<pair<X,Y>>& points)
```

`xs[k]` と `ys[k]`、または `points[k]` を点 id `k` として version 0 を構築する。

## バージョン・座標・更新

```cpp
int wm.size() const
int wm.versions() const
int wm.latest_version() const
X wm.x(k) const
Y wm.y(version, k) const
int new_version = wm.set_y(version, k, y)
int new_version = wm.fork(version)
```

`set_y` は任意の `version` の点 `k` の y 座標だけを更新する。`fork` は指定 version と同じ
内容の新 version を作る。x 座標は全 version で不変である。

## 矩形・順序クエリ

```cpp
int wm.rectangle_count(version, xl, xr, yl, yr) const
int wm.range_freq(version, xl, xr, yl, yr) const
Y wm.kth_smallest_y(version, xl, xr, k) const
optional<Y> wm.prev_y(version, xl, xr, upper) const
optional<Y> wm.next_y(version, xl, xr, lower) const
```

- `rectangle_count`, `range_freq`: `[xl,xr) × [yl,yr)` に含まれる点数を返す。
- `kth_smallest_y`: x が `[xl,xr)` に入る点の y の 0-indexed な第 `k` 小要素を返す。
- `prev_y`: 対象 x 範囲で `upper` 未満の最大 y。存在しなければ `nullopt`。
- `next_y`: 対象 x 範囲で `lower` 以上の最小 y。存在しなければ `nullopt`。

## 時間計算量

`N = size()`、`D = Y_BIT_WIDTH`、version 数を `V`、実際の `set_y` 回数を `U` とする。

- constructor: `O(N log N + D N)`
- `size`, `versions`, `latest_version`, `x`: `O(1)`
- `y`: `O(D log(N+1))`
- `fork`: `O(D)`
- `set_y`: `O(D log(N+1))`
- `rectangle_count`, `range_freq`: `O(log(N+1) + D log(N+1))`
- `kth_smallest_y`, `prev_y`, `next_y`: `O(log(N+1) + D log(N+1))`

全メモリは `O(D N + U D log(N+1) + V D + N)`。

## 注意点

矩形は半開区間。点 id と順序統計は 0-indexed。作成済み version は後の更新で変化しない。
不正な version・点・矩形・順序、vector 長不一致、点数・version 容量、y の bit 幅違反では
`runtime_error`。失敗した更新は新 version を作らない。copy は禁止、move は可能。
標準 C++20 で実装され、GCC 13 で利用できる。
