---
title: Dynamic Weighted Wavelet Matrix (動的重み付きウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/dynamic/dynamic_weighted_wavelet_matrix.hpp
---

整数列への挿入・削除・置換と、各要素に付随する重みの更新を扱う動的な重み付きウェーブレット行列。固定容量の葉を持つ決定的な平衡 B+ 木を各 bit 階層に持つため、列全体の再構築や平方分割を行わない。

## テンプレート引数

```cpp
DynamicWeightedWaveletMatrix<T, W, MAX_SIZE = -1, BIT_WIDTH = (T の bit 数)>
```

- `T`: 値の整数型
- `W`: 重みの型。デフォルト構築（零元）、加算、減算が必要
- `MAX_SIZE`: 列長の上限。省略時（`-1`）は上限なし（`int` の範囲内）。内部は B+ 木なので、指定しても確保する領域は変わらない。
- `BIT_WIDTH`: 値に使う bit 数。省略時は `T` の bit 数。符号付きの `T` では省略しなければならない（`static_assert`）。

```cpp
using Variable = DynamicWeightedWaveletMatrix<T, W>;
using Fixed = DynamicWeightedWaveletMatrix<T, W, N>;
```

## 構築と取得

```cpp
DynamicWeightedWaveletMatrix()
DynamicWeightedWaveletMatrix(const vector<T>& values, const vector<W>& weights)
DynamicWeightedWaveletMatrix(const array<T, N>& values, const array<W, N>& weights)
DynamicWeightedWaveletMatrix(const DynamicWeightedWaveletMatrix&)
DynamicWeightedWaveletMatrix(DynamicWeightedWaveletMatrix&&) noexcept
DynamicWeightedWaveletMatrix& operator=(const DynamicWeightedWaveletMatrix&)
DynamicWeightedWaveletMatrix& operator=(DynamicWeightedWaveletMatrix&&) noexcept
int size() const
T access(int k) const
T operator[](int k) const
W weight(int k) const
```

`values[k]` に重み `weights[k]` を対応させて構築する。2 つの列の長さは一致しなければならない。`access` / `operator[]` は `k` 番目の値を、`weight` は `k` 番目の重みを返す。

## 更新

```cpp
void insert(int position, T value, const W& weight)
pair<T, W> erase(int position)
void push_back(T value, const W& weight)
pair<T, W> pop_back()
void set(int k, T value, const W& weight)
void set_value(int k, T value)
void set_weight(int k, const W& weight)
```

`insert` は `position` の直前に挿入する（`position == size()` なら末尾）。`erase` と `pop_back` は削除した `(value, weight)` を返す。`set` は値と重みの両方を、`set_value` と `set_weight` は片方だけを置き換える。

## 順序統計

```cpp
int rank(T value, int r) const
int rank(T value, int l, int r) const
int select(T value, int occurrence) const
T kth_smallest(int l, int r, int k) const
T kth_largest(int l, int r, int k) const
int range_freq(int l, int r, T upper) const
int range_freq(int l, int r, T lower, T upper) const
optional<T> prev_value(int l, int r, T upper) const
optional<T> next_value(int l, int r, T lower) const
```

`rank` は区間 $[0, r)$ または $[l, r)$ にある `value` の個数を返す。`select` は `value` の `occurrence` 番目（0-indexed）の出現位置を返し、存在しなければ `size()` を返す。`kth_smallest` / `kth_largest` は区間 $[l, r)$ で `k` 番目（0-indexed）に小さい値 / 大きい値を返す。`range_freq` は `upper` 未満、または $[\mathit{lower}, \mathit{upper})$ に含まれる値の個数を返す。`prev_value` は `upper` 未満の最大値を、`next_value` は `lower` 以上の最小値を返し、存在しなければ `nullopt` を返す。

## 重みの集約

```cpp
W sum(int l, int r) const
W range_sum(int l, int r, T upper) const
W range_sum(int l, int r, T lower, T upper) const
W sum_k_smallest(int l, int r, int k) const
W sum_k_largest(int l, int r, int k) const
```

`sum` は区間 $[l, r)$ の重みの総和を、`range_sum` は値が `upper` 未満、または $[\mathit{lower}, \mathit{upper})$ に含まれる要素の重みの総和を返す。`sum_k_smallest` / `sum_k_largest` は区間を値の小さい順 / 大きい順に並べたときの先頭 `k` 個（$0 \le k \le r - l$）の重みの総和を返す。値が等しい要素がその境界をまたぐ場合は、現在の列における位置の順に選ぶ。

## 時間計算量

現在の列長を $N$、$D = \texttt{BIT_WIDTH}$ とする。動的ビットベクトルは決定的な平衡 B+ 木なので、以下は最悪計算量である。

- コンストラクタ（`vector` / `array`）: $O(DN)$
- コピー構築・コピー代入: $O(DN)$
- ムーブ構築・ムーブ代入: $O(D)$
- デフォルトコンストラクタ、`size`: $O(1)$
- `weight`, `sum`: $O(\log N)$
- `access`, `operator[]`: $O(D \log N)$
- `insert`, `erase`, `push_back`, `pop_back`: $O(D \log N)$
- `set`, `set_value`, `set_weight`: $O(D \log N)$
- `rank`, `select`, `range_freq`: $O(D \log N)$
- `kth_smallest`, `kth_largest`, `prev_value`, `next_value`: $O(D \log N)$
- `range_sum`, `sum_k_smallest`, `sum_k_largest`: $O(D \log N)$

## 空間計算量

$O(DN)$。

## 注意点

- 添字と出現番号は 0-indexed、列の区間と値域はそれぞれ半開区間 $[l, r)$, $[\mathit{lower}, \mathit{upper})$ である。
- 列長が `MAX_SIZE`（省略時は `int` の最大値）を超える操作、値が `BIT_WIDTH` bit に収まらない場合、範囲外の添字・区間・`k`、負の出現番号、`lower > upper`、空列からの `pop_back` では `std::runtime_error` を送出する。
- クエリに渡す `upper` / `lower` / `value` は `BIT_WIDTH` bit に収まらなくてもよい（全要素より大きいものとして扱う）。
