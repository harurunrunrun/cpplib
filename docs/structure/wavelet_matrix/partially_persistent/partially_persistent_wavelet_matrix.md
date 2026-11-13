---
title: Partially Persistent Wavelet Matrix (部分永続ウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/partially_persistent/partially_persistent_wavelet_matrix.hpp
---

固定長の整数列を点更新できる部分永続ウェーブレット行列。更新の対象を最新版に限定し、任意の過去の版に対してクエリできる。完全永続版のラッパーではなく、決定的な平衡 B+ 木のビット列を直接使う独立した実装である。

## テンプレート引数

```cpp
PartiallyPersistentWaveletMatrix<T, MAX_SIZE = -1, MAX_VERSION = -1, BIT_WIDTH = (T の bit 数)>
```

- `T`: `bool` 以外の整数型
- `MAX_SIZE`: 初期列の長さの上限。`-1` なら上限なし。
- `MAX_VERSION`: 版 0 の後に実行できる更新の回数。`-1` なら上限なし。
- `BIT_WIDTH`: 値を表す bit 数。省略時は `T` の bit 数。符号付き整数では省略しなければならない（`static_assert`）。

`MAX_SIZE` と `MAX_VERSION` は検査用の上限であり、内部の B+ 木と版の記録（`std::deque`）は必要に応じて伸長するため、指定しても確保する領域は変わらない。

```cpp
using Variable = PartiallyPersistentWaveletMatrix<T>;
using Fixed = PartiallyPersistentWaveletMatrix<T, N, Versions>;
```

## コンストラクタ

```cpp
PartiallyPersistentWaveletMatrix()
explicit PartiallyPersistentWaveletMatrix(const vector<T>& values)
explicit PartiallyPersistentWaveletMatrix(const array<T, N>& values)
```

`values` を版 0 として構築する。デフォルトコンストラクタは空列を構築する。

## 版と更新

```cpp
int size() const
int versions() const
int latest_version() const
int set(int k, T value)
```

- `size`: 列長を返す。
- `versions`: 現在存在する版の数を返す。
- `latest_version`: 最後に作成した版の番号を返す。
- `set`: 最新版の `k` 番目だけを `value` に置き換えた新しい版を作り、その番号を返す。

## クエリ

```cpp
T access(int version, int k) const
int rank(int version, T value, int r) const
int rank(int version, T value, int l, int r) const
int select(int version, T value, int occurrence) const
T kth_smallest(int version, int l, int r, int k) const
T kth_largest(int version, int l, int r, int k) const
int range_freq(int version, int l, int r, T upper) const
int range_freq(int version, int l, int r, T lower, T upper) const
optional<T> prev_value(int version, int l, int r, T upper) const
optional<T> next_value(int version, int l, int r, T lower) const
```

いずれも版 `version` の列に対するクエリである。

- `access`: `k` 番目の値を返す。
- `rank`: 区間 $[l, r)$ または $[0, r)$ に含まれる `value` の個数を返す。
- `select`: 列全体で `value` の `occurrence` 番目（0-indexed）の出現位置を返す。存在しなければ `size()` を返す。
- `kth_smallest`, `kth_largest`: 区間 $[l, r)$ で `k` 番目（0-indexed）に小さい値 / 大きい値を返す。
- 上限のみを取る `range_freq`: 区間 $[l, r)$ に含まれる `upper` 未満の値の個数を返す。
- 下限と上限を取る `range_freq`: 区間 $[l, r)$ に含まれる $[\mathit{lower}, \mathit{upper})$ の値の個数を返す。
- `prev_value`: `upper` 未満の最大値を返す。存在しなければ `nullopt` を返す。
- `next_value`: `lower` 以上の最小値を返す。存在しなければ `nullopt` を返す。

## 時間計算量

$N$ を列長、$D = \texttt{BIT_WIDTH}$、作成済みの版の数を $V$ とする。B+ 木の葉の容量と分岐数は定数である。

- コンストラクタ: $O(DN)$
- `size`, `versions`, `latest_version`: $O(1)$
- `set`: $O(D \log(N + 1))$
- `access`, `rank`, `select`: $O(D \log(N + 1))$
- `kth_smallest`, `kth_largest`: $O(D \log(N + 1))$
- `range_freq`, `prev_value`, `next_value`: $O(D \log(N + 1))$

## 空間計算量

初期構築のメモリは $O(DN)$ である。`set` を実行した回数を $U$ とすると、永続ノードと各版の根を含む全メモリは $O(DN + UD \log(N + 1) + VD)$ となる。

## 注意点

- 更新は常に `latest_version()` に対して行う。過去の版は読み取り専用で、その内容は以後の更新でも変化しない。
- 版の番号・位置・出現番号・順序統計は 0-indexed である。列の区間は半開区間 $[l, r)$、値域は $[\mathit{lower}, \mathit{upper})$ である。
- 不正な版・位置・区間・`k`、`lower > upper`、初期列長・版数の上限超過、`BIT_WIDTH` bit に収まらない値では `std::runtime_error` を送出する。失敗した `set` は全 bit 段のノード使用量を更新前に戻し、新しい版を作らない。
