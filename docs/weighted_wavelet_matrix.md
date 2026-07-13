---
title: Weighted Wavelet Matrix (重み付きウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/weighted_wavelet_matrix.hpp
---

各要素に重みを持つ静的wavelet matrix。値で絞り込んだ重みの総和を求める。

# テンプレート引数

```cpp
WeightedWaveletMatrix<T, W, MAX_SIZE, BIT_WIDTH>
```

- 整数型 `T`
- 重みの型 `W`
- 配列長の上限 `MAX_SIZE`
- 使用するbit数 `BIT_WIDTH`。省略時は `T` のbit数

符号付き整数では `BIT_WIDTH` を省略する。

# コンストラクタ

```cpp
WeightedWaveletMatrix(const vector<T>& values, const vector<W>& weights)
```

`values[k]` に重み `weights[k]` を対応させて構築する。

## 時間計算量

- $O(\mathrm{BIT\_WIDTH}\cdot\mathtt{MAX\_SIZE})$

# 要素取得

```cpp
T access(int k) const
T operator[](int k) const
```

## 時間計算量

- $O(\mathrm{BIT\_WIDTH})$

# rank / k番目

```cpp
int rank(T value, int r) const
int rank(T value, int l, int r) const
T kth_smallest(int l, int r, int k) const
T kth_largest(int l, int r, int k) const
```

`k` は0-indexed。

## 時間計算量

- $O(\mathrm{BIT\_WIDTH})$

# 頻度

```cpp
int range_freq(int l, int r, T upper) const
int range_freq(int l, int r, T lower, T upper) const
```

区間 $[l,r)$ にある `upper` 未満、または `[lower, upper)` の値の個数を返す。

## 時間計算量

- $O(\mathrm{BIT\_WIDTH})$

# 重みの総和

```cpp
W sum(int l, int r) const
W range_sum(int l, int r, T upper) const
W range_sum(int l, int r, T lower, T upper) const
W sum_k_smallest(int l, int r, int k) const
W sum_k_largest(int l, int r, int k) const
```

`sum` は区間 $[l,r)$ の全重みの総和を返す。`range_sum` は値が `upper` 未満、または `[lower, upper)` に入る要素の重みの総和を返す。

`sum_k_smallest` は区間 $[l,r)$ の小さい方から `k` 個の重みの総和を返す。`sum_k_largest` は大きい方から `k` 個。

## 時間計算量

- `sum`: $O(1)$
- それ以外: $O(\mathrm{BIT\_WIDTH})$

# 前後の値

```cpp
optional<T> prev_value(int l, int r, T upper) const
optional<T> next_value(int l, int r, T lower) const
```

区間内の `upper` 未満の最大値、または `lower` 以上の最小値を返す。存在しない場合は `nullopt`。

## 時間計算量

- $O(\mathrm{BIT\_WIDTH})$

# 計算量（公開操作別）

$D=\mathtt{BIT\_WIDTH}$ とする。重み用固定長配列も各levelに保持する。

- vector/array constructor: $O(D\mathtt{MAX\_SIZE})$
- `size`, `sum`: $O(1)$
- `access`, `operator[]`, `rank`, `kth_smallest`, `kth_largest`, `range_freq`, `range_sum`, `sum_k_smallest`, `sum_k_largest`, `prev_value`, `next_value`: $O(D)$

# 構築・共通契約

constructorは上記の値列と重み列を構築し、両vectorの長さは一致する必要がある。
`size` は列長、`access/operator[]` は値、`weight(k)` は1点の重みを返す。
点・出現番号・順序統計は0-indexed、列区間は `[l,r)`、値域は `[lower,upper)`。
`sum/range_sum` は全値域・指定値域の重み、`sum_k_smallest/sum_k_largest` は
順序先頭 `k` 個の重みを返す。前後値は該当なしなら `nullopt`。

vector長不一致、列長容量、値のbit幅、点・区間・順序・値域では `runtime_error`。
copyは禁止、moveは可能。各APIの計算量は上記表の通り。
