---
title: Partially Persistent Wavelet Matrix (部分永続ウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/partially_persistent_wavelet_matrix.hpp
---

点更新できる部分永続 Wavelet Matrix。
更新対象は最新バージョンに限る。過去のバージョンはクエリで参照できる。

## コンストラクタ

```cpp
PartiallyPersistentWaveletMatrix<T, MAX_SIZE, MAX_VERSION, BIT_WIDTH, BLOCK_SIZE> wm(a)
```

初期状態のバージョンは `0`。
`MAX_VERSION` 回まで更新できる。

## 更新

```cpp
int version = wm.set(k, value)
```

最新版の `a[k]` を `value` に変更し、新しいバージョン番号を返す。

## バージョン

```cpp
int count = wm.versions()
int version = wm.latest_version()
```

## クエリ

```cpp
T value = wm.access(version, k)
int count = wm.rank(version, value, l, r)
int index = wm.select(version, value, k)
T value = wm.kth_smallest(version, l, r, k)
T value = wm.kth_largest(version, l, r, k)
int count = wm.range_freq(version, l, r, upper)
int count = wm.range_freq(version, l, r, lower, upper)
std::optional<T> value = wm.prev_value(version, l, r, upper)
std::optional<T> value = wm.next_value(version, l, r, lower)
```

区間は半開区間 `[l, r)`。
`select` は `value` の `k` 番目の位置を返し、存在しなければ `size()` を返す。

## 時間計算量と追加メモリ

`B = BLOCK_SIZE`、区間が触れるブロック数を `C`、バージョン数を `V` とする。

- `set`: `O(B log B)`
- `access`: 最新versionは $O(1)$、過去versionは $O(\log V)$
- `rank`, `range_freq`: 最新versionは $O(B+C\log B)$、過去versionは $O(B+C(\log B+\log V))$
- `kth_smallest`, `kth_largest`: 上記にそれぞれ $\mathtt{BIT\_WIDTH}$ を掛けた時間
- 追加メモリ: `O(N + V B)`

更新では変更されたブロックだけを複製する。

## 公開操作別の詳細

$M=\lceil N/B\rceil$、長さ $L$ の区間が触れるblock数を $C$ とし、
$Q_0(L)=B+C\log B$、$Q_V(L)=B+C(\log B+\log(V+1))$ とおく。

- default/vector/array constructor: 固定容量block配列を含めて $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSION}B+N\log B)$
- `size`, `versions`, `latest_version`: $O(1)$
- `access`: 最新versionは $O(1)$、過去versionは $O(\log(V+1))$
- `set`: 償却 $O(B\log B)$。block履歴vectorの再確保が起きる1回の最悪は $O(B\log B+V)$
- `rank`, `range_freq`: 最新versionは $O(Q_0(L))$、過去versionは $O(Q_V(L))$
- `select`: 最新versionは $O(Q_0(N)\log N)$、過去versionは $O(Q_V(N)\log N)$
- `kth_smallest`, `kth_largest`, `prev_value`, `next_value`: 最新versionは $O(\mathtt{BIT\_WIDTH}\,Q_0(L))$、過去versionは $O(\mathtt{BIT\_WIDTH}\,Q_V(L))$

## 注意点

version 0が初期列。更新は `latest_version()` からだけ新versionを作り、その番号を返す。
queryは先頭のversion引数で任意の有効versionを参照する。
`size/versions/latest_version` は列長・version数・最新番号を返す。

点・順序は0-indexed、列区間は半開区間 `[l,r)`、値域は `[lower,upper)`。
`select` は該当なしなら `size()`、前後値は `nullopt`。
不正なversion・点・区間・順序・値域、bit幅、容量超過では `runtime_error`。
各APIの計算量は上記表の通り。

## Constructor signature

```cpp
PartiallyPersistentWaveletMatrix()
PartiallyPersistentWaveletMatrix(const vector<T>& values)
PartiallyPersistentWaveletMatrix(const array<T, N>& values)
```
