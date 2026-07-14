---
title: Offline Persistent Union-Find Queries (オフライン完全永続Union-Findクエリ) [persistent_unionfind]
documentation_of: ../src/structure/dsu/offline_persistent_unionfind_queries.hpp
---

各操作が過去の操作直後の状態を参照するUnion-Findクエリを登録し、連結性の答えを
まとめて計算する。操作番号は追加順の0始まりで、`-1` は初期状態を表す。

# テンプレート引数

```cpp
OfflinePersistentUnionFindQueries<MAX_SIZE, MAX_OPERATION, MAX_QUERY>
```

`MAX_SIZE` は頂点数、`MAX_OPERATION` は操作数、`MAX_QUERY` は `same` の
呼び出し回数の上限。copyはできず、moveはできる。

# コンストラクタ

```cpp
OfflinePersistentUnionFindQueries(int size = MAX_SIZE)
```

互いに素な `size` 頂点を持つ初期状態を作る。

## 時間計算量

$O(MAX\_SIZE+MAX\_OPERATION)$。

# `merge`

```cpp
void merge(int base_operation, int left, int right)
```

`base_operation` の操作直後の状態から `left` と `right` を併合した状態を、
次の操作として追加する。`base_operation == -1` なら初期状態を参照する。

## 時間計算量

amortized $O(1)$。

# `same`

```cpp
int same(int base_operation, int left, int right)
```

指定状態で2頂点が同じ成分かを問う操作を追加する。この操作直後の状態は指定状態と
同じ。返り値は0始まりの連結性query番号である。

## 時間計算量

amortized $O(1)$。

# `solve`

```cpp
vector<int> solve()
```

`same` の登録順に答えを `0` または `1` で返す。同じ登録内容に対して複数回
呼び出せる。

## 時間計算量

操作数を $M$、`same` の個数を $Q$、頂点数を $N$ として
$O((M+Q)\log N)$。

# `size` / `operations` / `query_count`

```cpp
int size() const
int operations() const
int query_count() const
```

頂点数、登録操作数、登録済み連結性query数を返す。

## 時間計算量

すべて $O(1)$。

# 空間計算量

$O(MAX\_SIZE+MAX\_OPERATION+MAX\_QUERY)$。

# 注意点

`base_operation` は `-1` または追加済み操作の番号でなければならない。
頂点、操作番号、固定容量の範囲外では `out_of_range` または `length_error` を
送出する。答えが必要になるまで `solve` を呼ぶ必要がある。
