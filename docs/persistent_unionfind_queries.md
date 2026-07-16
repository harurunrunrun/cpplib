---
title: Persistent Union-Find Queries (クエリ履歴付き完全永続素集合データ構造) [persistent_unionfind]
documentation_of: ../src/structure/dsu/persistent_unionfind_queries.hpp
---

各操作の番号をversion識別子として扱う完全永続Union-Find。
更新を行わない連結性queryも履歴へ登録するため、呼び出し側で内部version番号を管理する必要がない。

## テンプレート引数

```cpp
PersistentUnionFindQueries<MAX_SIZE, MAX_QUERY>
```

`MAX_SIZE` は頂点数の上限、`MAX_QUERY` は登録できる操作数の上限である。
固定長の永続データはheapに確保されるため、wrapperをlocal変数として構築しても巨大な配列をstackへ置かない。

## コンストラクタ

```cpp
explicit PersistentUnionFindQueries(int n = MAX_SIZE)
```

`n` 個の独立な頂点を持つ初期状態を作る。初期状態の操作番号は `-1`。

## 時間計算量

$O(MAX\_QUERY)$。

## `processed_queries`

```cpp
int processed_queries() const
```

登録済み操作数を返す。

## 時間計算量

$O(1)$。

## `merge`

```cpp
void merge(int base_query, int u, int v)
```

操作 `base_query` の直後の状態を基に `u` と `v` を併合し、現在の操作番号へ状態を登録する。

## 時間計算量

頂点数を $N$ として $O(\log N\log MAX\_SIZE)$。

## `same`

```cpp
bool same(int base_query, int u, int v)
```

操作 `base_query` の直後の状態で `u` と `v` が同じ成分かを返す。
状態を変更せず、同じ状態を現在の操作番号へ登録する。

## 時間計算量

$O(\log N\log MAX\_SIZE)$。

## 空間計算量

予約領域は $O(MAX\_QUERY\log MAX\_SIZE)$。wrapper本体は $O(1)$ であり、予約領域はheapに置かれる。

## 注意点

- `base_query == -1` は初期状態を表す。それ以外は `[0, processed_queries())` が必要。
- 頂点は `[0,n)`、操作数は `MAX_QUERY` 以下でなければならない。
- 範囲違反、操作容量超過、内部永続領域の容量超過では `runtime_error`。
- 失敗した操作は操作数を増やさない。
- copyは禁止し、moveを許可する。
