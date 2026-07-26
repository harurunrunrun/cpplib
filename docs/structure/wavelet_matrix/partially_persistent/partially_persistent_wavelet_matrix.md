---
title: Partially Persistent Wavelet Matrix (部分永続ウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/partially_persistent/partially_persistent_wavelet_matrix.hpp
---

固定長の整数列を点更新できる部分永続 Wavelet Matrix。
更新対象を最新 version に限定し、任意の過去 version をクエリできる。
完全永続版の wrapper ではなく、決定的平衡 B+ 木の bit sequence coreを直接使う独立実装である。

## テンプレート引数

```cpp
PartiallyPersistentWaveletMatrix<T, MAX_SIZE, MAX_VERSION, BIT_WIDTH>
```

- `T`: `bool` 以外の整数型。
- `MAX_SIZE`: 初期列の最大長。
- `MAX_VERSION`: version 0 の後に実行できる更新回数。
- `BIT_WIDTH`: 値を表す bit 数。省略時は `T` の全 bit 数。符号付き整数では全 bit 数を指定する。

## コンストラクタ

```cpp
PartiallyPersistentWaveletMatrix()
PartiallyPersistentWaveletMatrix(const vector<T>& values)
PartiallyPersistentWaveletMatrix(const array<T, N>& values)
```

`values` を version 0 として構築する。default constructor は空列を構築する。

## バージョンと更新

```cpp
int wm.size() const
int wm.versions() const
int wm.latest_version() const
int new_version = wm.set(k, value)
```

- `size`: 列長を返す。
- `versions`: 現在存在する version 数を返す。
- `latest_version`: 最後に作成した version 番号を返す。
- `set`: 最新 version の `k` 番目だけを `value` に置換し、新 version を返す。

## クエリ

```cpp
T wm.access(version, k) const
int wm.rank(version, value, r) const
int wm.rank(version, value, l, r) const
int wm.select(version, value, occurrence) const
T wm.kth_smallest(version, l, r, k) const
T wm.kth_largest(version, l, r, k) const
int wm.range_freq(version, l, r, upper) const
int wm.range_freq(version, l, r, lower, upper) const
optional<T> wm.prev_value(version, l, r, upper) const
optional<T> wm.next_value(version, l, r, lower) const
```

- `access`: 指定位置の値を返す。
- `rank`: `[l,r)`、または `[0,r)` に含まれる `value` の個数を返す。
- `select`: 列全体で `value` が `occurrence` 回現れる位置を返す。存在しなければ `size()`。
- `kth_smallest`, `kth_largest`: `[l,r)` の 0-indexed な第 `k` 要素を返す。
- 1引数の `range_freq`: `[l,r)` に含まれる `upper` 未満の値の個数を返す。
- 2引数の `range_freq`: `[l,r)` に含まれる `[lower,upper)` の値の個数を返す。
- `prev_value`: `upper` 未満の最大値を返す。存在しなければ `nullopt`。
- `next_value`: `lower` 以上の最小値を返す。存在しなければ `nullopt`。

## 時間計算量

`N = size()`、`D = BIT_WIDTH`、作成済み version 数を `V` とする。
B+ 木の葉容量と分岐数は定数である。

- constructor: `O(D N)`
- `size`, `versions`, `latest_version`: `O(1)`
- `set`: `O(D log(N+1))`
- `access`, `rank`, `select`: `O(D log(N+1))`
- `kth_smallest`, `kth_largest`: `O(D log(N+1))`
- `range_freq`, `prev_value`, `next_value`: `O(D log(N+1))`

初期構築のメモリは `O(D N)`。更新回数を `U` とすると、過去 version の root を含む
全メモリは `O(D N + U D log(N+1) + V D)`。

## 注意点

更新は常に `latest_version()` に対して行う。過去 version は読み取り専用で、その内容は
以後の更新でも変化しない。version、位置、出現番号、順序統計は 0-indexed。列区間は
半開区間 `[l,r)`、値域は `[lower,upper)`。

不正な version・位置・区間・順序・値域、初期列長・version 容量・bit 幅の違反では
`runtime_error`。失敗した `set` は全 bit 段のノード使用量を更新前へ戻し、新 version を
作らない。標準 C++20 で実装され、GCC 13 で利用できる。
