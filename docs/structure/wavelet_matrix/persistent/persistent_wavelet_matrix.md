---
title: Persistent Wavelet Matrix (完全永続ウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/persistent/persistent_wavelet_matrix.hpp
---

固定長の整数列を点更新できる完全永続 Wavelet Matrix。
任意の過去 version から新しい version を作成でき、作成済みの version は変更されない。
各 bit 段を決定的平衡 B+ 木で保持するため、乱数や平方分割の調整値に依存しない。

## テンプレート引数

```cpp
PersistentWaveletMatrix<T, MAX_SIZE, MAX_VERSION, BIT_WIDTH>
```

- `T`: `bool` 以外の整数型。
- `MAX_SIZE`: 初期列の最大長。
- `MAX_VERSION`: 初期 version 0 の後に作成できる version 数。
- `BIT_WIDTH`: 値を表す bit 数。省略時は `T` の全 bit 数。符号付き整数では全 bit 数を指定する。

## コンストラクタ

```cpp
PersistentWaveletMatrix()
PersistentWaveletMatrix(const vector<T>& values)
PersistentWaveletMatrix(const array<T, N>& values)
```

`values` を version 0 として構築する。default constructor は空列を構築する。

## バージョンと更新

```cpp
int wm.size() const
int wm.versions() const
int wm.latest_version() const
int new_version = wm.set(version, k, value)
int new_version = wm.fork(version)
```

- `size`: 列長を返す。
- `versions`: 現在存在する version 数を返す。
- `latest_version`: 最後に作成した version 番号を返す。
- `set`: `version` の `k` 番目だけを `value` に置換した新 version を返す。
- `fork`: `version` と同じ内容を共有する新 version を返す。

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
- `fork`: `O(D)`
- `set`: `O(D log(N+1))`
- `access`, `rank`, `select`: `O(D log(N+1))`
- `kth_smallest`, `kth_largest`: `O(D log(N+1))`
- `range_freq`, `prev_value`, `next_value`: `O(D log(N+1))`

初期構築のメモリは `O(D N)`。実際に `set` した回数を `U` とすると、永続ノードと
version root を含む全メモリは `O(D N + U D log(N+1) + V D)`。
`fork` は bit 列ノードを複製しない。

## 注意点

version、位置、出現番号、順序統計は 0-indexed。列区間は半開区間 `[l,r)`、値域は
`[lower,upper)`。作成済み version への query と分岐は、その後の更新の影響を受けない。

不正な version・位置・区間・順序・値域、初期列長・version 容量・bit 幅の違反では
`runtime_error`。失敗した `set` は全 bit 段のノード使用量を更新前へ戻し、新 version を
作らない。標準 C++20 で実装され、GCC 13 で利用できる。
