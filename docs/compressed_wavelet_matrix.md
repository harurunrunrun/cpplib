---
title: Compressed Wavelet Matrix (座標圧縮ウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/compressed_wavelet_matrix.hpp
---

値を座標圧縮して扱う静的wavelet matrix。任意の比較可能な型を値として使える。

# テンプレート引数

```cpp
CompressedWaveletMatrix<T, MAX_SIZE, ID_BIT_WIDTH>
```

- 値の型 `T`
- 配列長の上限 `MAX_SIZE`
- 圧縮後のidに使用するbit数 `ID_BIT_WIDTH`

`ID_BIT_WIDTH` は省略できる。

# コンストラクタ

```cpp
CompressedWaveletMatrix(const vector<T>& values)
```

`values` を座標圧縮して構築する。

## 時間計算量

- $O(N\log N+\mathtt{MAX\_SIZE}+\mathrm{ID\_BIT\_WIDTH}(N+\lceil\mathtt{MAX\_SIZE}/64\rceil))$

# 要素取得

```cpp
T access(int k) const
T operator[](int k) const
```

# rank / select

```cpp
int rank(const T& value, int r) const
int rank(const T& value, int l, int r) const
int select(const T& value, int k) const
```

`select` は存在しない場合 `size()` を返す。

# k番目 / 中央値

```cpp
T kth_smallest(int l, int r, int k) const
T kth_largest(int l, int r, int k) const
T quantile(int l, int r, int k) const
T range_min(int l, int r) const
T range_max(int l, int r) const
T median_floor(int l, int r) const
T median_ceil(int l, int r) const
```

`k` は0-indexed。

# 頻度

```cpp
int range_freq(int l, int r, const T& upper) const
int range_freq(int l, int r, const T& lower, const T& upper) const
```

区間 $[l,r)$ にある `upper` 未満、または `[lower, upper)` の値の個数を返す。

# 前後の値

```cpp
optional<T> prev_value(int l, int r, const T& upper) const
optional<T> next_value(int l, int r, const T& lower) const
```

## 時間計算量

- 構築以外: $O(\log N + \mathrm{ID\_BIT\_WIDTH})$

# 計算量（公開操作別）

$D$ を異なる値の個数、$H=\mathtt{ID\_BIT\_WIDTH}$、$W=\lceil\mathtt{MAX\_SIZE}/64\rceil$ とする。

- vector/array constructor: $O(N\log N+\mathtt{MAX\_SIZE}+H(N+W))$
- `size`, `value_count`, `value`: $O(1)$
- `access`, `operator[]`, `kth_smallest`, `kth_largest`, `quantile`, `range_min`, `range_max`, `median_floor`, `median_ceil`: $O(H)$
- `rank`, `range_freq`, `prev_value`, `next_value`: $O(\log D+H)$
- `select`: $O(\log D+H\log N)$

# 構築・共通契約

constructorは上記の値列を座標圧縮して構築する。`size` は列長、`value_count` は異なる値数、
`value(id)` は圧縮idの元の値を返す。`access/operator[]`、出現番号、順序統計は0-indexed。
列区間は `[l,r)`、値域は `[lower,upper)`。`quantile` は `kth_smallest` の別名。
`range_min/max` と `median_floor/ceil` は空でない区間が前提。前後値は該当なしなら `nullopt`。

列長容量、異なる値数に対するID bit幅、点・区間・圧縮id・順序・値域の違反では
`runtime_error`。copyは禁止、moveは可能。各APIの計算量は上記表の通り。
