---
title: Offline Persistent Disjoint Set Union (オフライン完全永続素集合データ構造)
documentation_of: ../src/structure/dsu/offline_persistent_dsu.hpp
---

version treeを先に構築し、rollbackによる1回の走査で各versionの連結性queryを
まとめて処理する完全永続Union-Find。

## テンプレート引数

```cpp
OfflinePersistentDSU<MAX_SIZE, MAX_VERSION, MAX_QUERY>
```

`MAX_SIZE` は頂点数、`MAX_VERSION` は初期versionを除くversion数、
`MAX_QUERY` は登録query数の上限。copyはできず、moveはできる。
`MAX_VERSION <= INT_MAX / 2`、`MAX_QUERY < INT_MAX` が必要。

## コンストラクタ

```cpp
OfflinePersistentDSU(int size = MAX_SIZE)
```

互いに素な `size` 頂点を持つversion `0` を作る。

## 時間計算量

$O(MAX\_SIZE+MAX\_VERSION)$。

## `merge` / `fork`

```cpp
int merge(int version, int left, int right)
int fork(int version)
```

`merge` は指定versionで2頂点を併合した新version、`fork` は内容が同じ新versionを
追加し、その番号を返す。実際の併合は `solve` まで遅延する。

## 時間計算量

各呼び出しamortized $O(1)$。

## `query_same`

```cpp
int query_same(int version, int left, int right)
```

指定versionで2頂点が同じ成分かを問うqueryを登録し、0始まりのquery番号を返す。

## 時間計算量

amortized $O(1)$。

## `solve`

```cpp
vector<int> solve()
```

登録順に `query_same` の答えを `0` または `1` で返す。同じ内容に対して複数回
呼び出せる。

## 時間計算量

作成version数を $V$、query数を $Q$、頂点数を $N$ として
$O((V+Q)\log N)$。

## 空間計算量

本体は $O(MAX\_SIZE+MAX\_VERSION+MAX\_QUERY)$、`solve` の返り値と
走査stackは $O(V+Q)$。

## `size` / `versions` / `query_count`

```cpp
int size() const
int versions() const
int query_count() const
```

頂点数、初期versionを含むversion数、登録query数を返す。すべて $O(1)$。

## 注意点

頂点、version、各固定容量の範囲外では `out_of_range` または `length_error`。
Union by sizeを使いpath compressionは行わない。
