---
title: Partially Persistent Weighted Wavelet Matrix (部分永続重み付きウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/partially_persistent_weighted_wavelet_matrix.hpp
---

各要素に重みを持つ、点更新可能な部分永続 Wavelet Matrix。
更新対象は最新バージョンに限る。過去のバージョンはクエリで参照できる。

## コンストラクタ

```cpp
PartiallyPersistentWeightedWaveletMatrix<
    T, W, MAX_SIZE, MAX_VERSION, BIT_WIDTH, BLOCK_SIZE
> wm(values, weights)
```

初期状態のバージョンは `0`。
`MAX_VERSION` 回まで更新できる。

## 更新

```cpp
int version = wm.set(k, value, weight)
int version = wm.set_value(k, value)
int version = wm.set_weight(k, weight)
```

最新版を更新し、新しいバージョン番号を返す。

## バージョン

```cpp
int count = wm.versions()
int version = wm.latest_version()
```

## クエリ

```cpp
T value = wm.access(version, k)
W weight = wm.weight(version, k)
int count = wm.rank(version, value, l, r)
T value = wm.kth_smallest(version, l, r, k)
T value = wm.kth_largest(version, l, r, k)
int count = wm.range_freq(version, l, r, lower, upper)
W sum = wm.sum(version, l, r)
W sum = wm.range_sum(version, l, r, lower, upper)
W sum = wm.sum_k_smallest(version, l, r, k)
W sum = wm.sum_k_largest(version, l, r, k)
std::optional<T> value = wm.prev_value(version, l, r, upper)
std::optional<T> value = wm.next_value(version, l, r, lower)
```

区間は半開区間 `[l, r)`。
`range_sum` は値が `[lower, upper)` に入る要素の重みの総和を返す。

## 時間計算量と追加メモリ

`B = BLOCK_SIZE`、区間が触れるブロック数を `C`、バージョン数を `V` とする。

- `set`, `set_value`, `set_weight`: `O(B log B)`
- `access`, `weight`: 最新versionは $O(1)$、過去versionは $O(\log V)$
- `rank`, `range_freq`, `range_sum`: 最新versionは $O(B+C\log B)$、過去versionは $O(B+C(\log B+\log V))$
- `sum`: 最新versionは $O(B+C)$、過去versionは $O(B+C\log V)$
- `kth_smallest`, `kth_largest`: `rank`の時間に $\mathtt{BIT\_WIDTH}$ を掛けた時間
- 追加メモリ: `O(N + V B)`

更新では変更されたブロックだけを複製する。

## 公開操作別の詳細

$M=\lceil N/B\rceil$、長さ $L$ の区間が触れるblock数を $C$ とし、
$Q_0(L)=B+C\log B$、$Q_V(L)=B+C(\log B+\log(V+1))$、
$S_0(L)=B+C$、$S_V(L)=B+C\log(V+1)$ とおく。

- default/vector/array constructor: $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSION}B+N\log B)$
- `size`, `versions`, `latest_version`: $O(1)$
- `access`, `weight`: 最新versionは $O(1)$、過去versionは $O(\log(V+1))$
- `set`, `set_value`, `set_weight`: 償却 $O(B\log B)$。履歴vector再確保時の1回の最悪は $O(B\log B+V)$
- `rank`, `range_freq`, `range_sum`: 最新versionは $O(Q_0(L))$、過去versionは $O(Q_V(L))$
- `sum`: 最新versionは $O(S_0(L))$、過去versionは $O(S_V(L))$
- `kth_smallest`, `kth_largest`, `prev_value`, `next_value`: 最新versionは $O(\mathtt{BIT\_WIDTH}\,Q_0(L))$、過去versionは $O(\mathtt{BIT\_WIDTH}\,Q_V(L))$
- `sum_k_smallest`, `sum_k_largest`: 上記の順序統計時間に境界値走査の $O(L)$ を加えた時間

## 注意点

version 0が初期列。更新は `latest_version()` からだけ新versionを作り、その番号を返す。
queryは先頭のversion引数で任意の有効versionを参照する。
`size/versions/latest_version` は列長・version数・最新番号を返す。

点・順序は0-indexed、列区間は半開区間 `[l,r)`、値域は `[lower,upper)`。
前後値は該当なしなら `nullopt`。
不正なversion・点・区間・順序・値域、bit幅、容量超過では `runtime_error`。
各APIの計算量は上記表の通り。

`set` は値と重み、`set_value/set_weight` は片方を更新する。`weight` は1点の重み、
重みqueryの区間と値域も半開区間である。

## Constructor signature

```cpp
PartiallyPersistentWeightedWaveletMatrix()
PartiallyPersistentWeightedWaveletMatrix(const vector<T>& values, const vector<W>& weights)
PartiallyPersistentWeightedWaveletMatrix(const array<T, N>& values, const array<W, N>& weights)
```
