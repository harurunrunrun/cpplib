---
title: Offline Persistent Union-Find Queries (オフライン完全永続Union-Findクエリ) [persistent_unionfind]
documentation_of: ../../../../src/structure/dsu/persistent/offline_persistent_unionfind_queries.hpp
---

各操作が過去のある操作の直後の状態を参照する Union-Find のクエリを登録し、連結性の答えをまとめて計算する。
操作番号は追加順に $0$ から振られ、`-1` は初期状態を表す。内部では `OfflinePersistentDSU` を使う。

## テンプレート引数

```cpp
OfflinePersistentUnionFindQueries<MAX_SIZE = -1, MAX_OPERATION = -1, MAX_QUERY = MAX_OPERATION>
```

- `MAX_SIZE`: 頂点数の上限
- `MAX_OPERATION`: 操作数（`merge` と `same` の合計）の上限
- `MAX_QUERY`: `same` の呼び出し回数の上限（デフォルトは `MAX_OPERATION`）

省略（`-1`）した上限は、実行時の要素数に合わせて `std::vector` を確保する可変長になる。コピーはできず、ムーブはできる。

```cpp
using Dynamic = OfflinePersistentUnionFindQueries<>;
```

## コンストラクタ

```cpp
explicit OfflinePersistentUnionFindQueries(int size = MAX_SIZE)
```

互いに素な `size` 頂点を持つ初期状態を作る。可変長版では `size` のデフォルトは $0$ である。

## merge

```cpp
void merge(int base_operation, int left, int right)
```

操作 `base_operation` の直後の状態から `left` と `right` を併合した状態を、次の操作として追加する。`base_operation == -1` なら初期状態を参照する。

## same

```cpp
int same(int base_operation, int left, int right)
```

指定した状態で 2 頂点が同じ成分に属するかを問う操作を追加する。この操作の直後の状態は指定した状態と同じである。戻り値は $0$ 始まりの連結性クエリの番号である。

## solve

```cpp
vector<int> solve()
```

`same` の登録順に答えを `0` または `1` で返す。同じ登録内容に対して何度でも呼び出せる。

## size / operations / query_count

```cpp
int size() const
int operations() const
int query_count() const
```

頂点数、登録した操作数、登録済みの連結性クエリ数を返す。

## 時間計算量

操作数を $M$、`same` の個数を $Q$、頂点数を $N$ とする。

- コンストラクタ: $O(N)$。固定長版では配列の確保に $O(\texttt{MAX_OPERATION})$ かかる
- `merge`、`same`: 償却 $O(1)$
- `solve`: $O((M+Q)\log(N+1))$
- `size`、`operations`、`query_count`: $O(1)$

## 空間計算量

$O(N+M+Q)$。固定長版では $O(\texttt{MAX_SIZE}+\texttt{MAX_OPERATION}+\texttt{MAX_QUERY})$ を見込む。

## 注意点

- `base_operation` は `-1` または追加済みの操作の番号でなければならない。
- 頂点・操作番号が範囲外の場合は `std::out_of_range`、固定容量を超えた場合は `std::length_error` を送出する。
- 答えは `solve` を呼ぶまで計算されない。
