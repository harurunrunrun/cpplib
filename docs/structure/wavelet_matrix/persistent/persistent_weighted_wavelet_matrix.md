---
title: Persistent Weighted Wavelet Matrix (完全永続重み付きウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/persistent/persistent_weighted_wavelet_matrix.hpp
---

各要素に重みを持ち、任意の有効な版から点更新・分岐できる完全永続ウェーブレット行列。各 bit 段を重みの和付きの永続 B+ 木で管理し、古い版は変更しない。

## テンプレート引数

```cpp
PersistentWeightedWaveletMatrix<T, W, MAX_SIZE = -1, MAX_VERSION = -1, BIT_WIDTH = (T の bit 数)>
```

- `T`: `bool` 以外の整数型
- `W`: 重みの型。`W{}` を加法の単位元として、`W + W` と `W - W` が定義されている必要がある。
- `MAX_SIZE`: 列長の上限。`-1` なら上限なし。
- `MAX_VERSION`: 版 0 の後に作成できる版の数。`-1` なら上限なし。
- `BIT_WIDTH`: 値を表す bit 数。省略時は `T` の bit 数。符号付きの `T` では省略しなければならない（`static_assert`）。

`MAX_SIZE` と `MAX_VERSION` は検査用の上限であり、内部の B+ 木と版の記録（`std::deque`）は必要に応じて伸長するため、指定しても確保する領域は変わらない。

```cpp
using Variable = PersistentWeightedWaveletMatrix<T, W>;
using Fixed = PersistentWeightedWaveletMatrix<T, W, N, Versions>;
```

## コンストラクタ

```cpp
PersistentWeightedWaveletMatrix()
PersistentWeightedWaveletMatrix(const vector<T>& values, const vector<W>& weights)
PersistentWeightedWaveletMatrix(const array<T, N>& values, const array<W, N>& weights)
```

`values[k]` に重み `weights[k]` を対応させた列を版 0 として構築する。デフォルトコンストラクタは空列を構築する。

## 版と更新

```cpp
int size() const
int versions() const
int latest_version() const
int set(int version, int k, T value, W weight)
int set_value(int version, int k, T value)
int set_weight(int version, int k, W weight)
int fork(int version)
```

`set` は版 `version` の `k` 番目の値と重みを、`set_value` は値だけを、`set_weight` は重みだけを置き換えた新しい版を作り、その番号を返す。`fork` は版 `version` と同じ内容の新しい版を作る。

## クエリ

```cpp
T access(int version, int k) const
W weight(int version, int k) const
int rank(int version, T value, int r) const
int rank(int version, T value, int l, int r) const
int select(int version, T value, int occurrence) const
T kth_smallest(int version, int l, int r, int k) const
T kth_largest(int version, int l, int r, int k) const
int range_freq(int version, int l, int r, T upper) const
int range_freq(int version, int l, int r, T lower, T upper) const
W sum(int version, int l, int r) const
W range_sum(int version, int l, int r, T upper) const
W range_sum(int version, int l, int r, T lower, T upper) const
W sum_k_smallest(int version, int l, int r, int k) const
W sum_k_largest(int version, int l, int r, int k) const
optional<T> prev_value(int version, int l, int r, T upper) const
optional<T> next_value(int version, int l, int r, T lower) const
```

いずれも任意の有効な版 `version` を読む。

- `access` / `weight`: `k` 番目の値 / 重みを返す。
- `rank`, `select`, `kth_smallest`, `kth_largest`, `range_freq`, `prev_value`, `next_value`: Persistent Wavelet Matrix と同じ。`select` は `value` の `occurrence` 番目（0-indexed）の出現位置を返し、存在しなければ `size()` を返す。
- `sum`: 区間 $[l, r)$ の重みの総和を返す。
- `range_sum`: 区間 $[l, r)$ のうち、値が `upper` 未満、または $[\mathit{lower}, \mathit{upper})$ に含まれる要素の重みの総和を返す。
- `sum_k_smallest` / `sum_k_largest`: 区間 $[l, r)$ を値の小さい順 / 大きい順に並べたときの先頭 `k` 個（$0 \le k \le r - l$）の重みの総和を返す。値が等しい要素がその境界をまたぐ場合は、現在の列での位置の順に選ぶ。

## 時間計算量

$N$ を列長、$D = \texttt{BIT_WIDTH}$、$H = O(\log(N + 1))$ とする。

- コンストラクタ（`vector` / `array`）: $O(DN)$、デフォルトコンストラクタ: $O(D)$
- `size`, `versions`, `latest_version`: $O(1)$
- `weight`, `sum`: $O(H)$
- `access`, `rank`, `select`, `kth_smallest`, `kth_largest`: $O(DH)$
- `range_freq`, `range_sum`, `sum_k_smallest`, `sum_k_largest`: $O(DH)$
- `prev_value`, `next_value`: $O(DH)$
- `set`, `set_value`, `set_weight`: $O(DH)$
- `fork`: $O(D)$

## 空間計算量

初期の追加メモリは $O(DN)$、更新 1 回あたりの追加メモリは $O(DH)$、`fork` 1 回あたりは $O(D)$ である。

## 注意点

- 版の番号・位置・出現番号・順序統計は 0-indexed、区間と値域は半開区間である。
- 不正な版・添字・区間・`k`、`lower > upper`、`BIT_WIDTH` bit に収まらない値、列長・版数の上限超過では `std::runtime_error` を送出する。
- B+ 木の更新途中で容量確保や重みの演算が失敗した場合も、全 bit 段のノード使用量と版の数を更新前に戻す。
