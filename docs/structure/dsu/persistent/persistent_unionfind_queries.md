---
title: Persistent Union-Find Queries (クエリ履歴付き完全永続素集合データ構造) [persistent_unionfind]
documentation_of: ../../../../src/structure/dsu/persistent/persistent_unionfind_queries.hpp
---

各操作の番号をバージョンの識別子として扱う完全永続 Union-Find。
更新を行わない連結性クエリも履歴に登録するため、呼び出し側で内部のバージョン番号を管理する必要がない。内部では `PersistentDSU` を使う。

## テンプレート引数

```cpp
PersistentUnionFindQueries<MAX_SIZE = -1, MAX_QUERY = -1>
```

- `MAX_SIZE`: 頂点数の上限
- `MAX_QUERY`: 登録できる操作数（`merge` と `same` の合計）の上限

省略（`-1`）した上限は、実行時の要素数・操作数に合わせて `std::vector` を確保する可変長になる。
固定長の永続データはヒープに確保されるため、この構造体をローカル変数として構築しても巨大な配列をスタックに置かない。

```cpp
using Dynamic = PersistentUnionFindQueries<>;
```

## コンストラクタ

```cpp
explicit PersistentUnionFindQueries(int n = MAX_SIZE)
```

$n$ 個の独立な頂点を持つ初期状態を作る。初期状態の操作番号は `-1` である。可変長版では `n` のデフォルトは $0$ である。

## processed_queries

```cpp
int processed_queries() const
```

登録済みの操作数を返す。

## merge

```cpp
void merge(int base_query, int u, int v)
```

操作 `base_query` の直後の状態をもとに $u$ と $v$ を併合し、その状態を現在の操作番号に登録する。

## same

```cpp
bool same(int base_query, int u, int v)
```

操作 `base_query` の直後の状態で $u$ と $v$ が同じ成分に属するかを返す。状態は変更せず、同じ状態を現在の操作番号に登録する。

## 時間計算量

$N$ を頂点数、$S$ を永続配列の幅（固定長版では $\texttt{MAX_SIZE}$、可変長版では $N$）とする。

- コンストラクタ: 固定長版は $O(\texttt{MAX_QUERY})$、可変長版は $O(1)$
- `processed_queries`: $O(1)$
- `merge`、`same`: $O(\log N \log S)$

## 空間計算量

固定長版の予約領域は $O(\texttt{MAX_QUERY}\log \texttt{MAX_SIZE})$ で、ヒープに置かれる。構造体本体は $O(1)$ である。
可変長版ではバージョン管理に操作数に比例する領域を使い、永続配列の領域は `PersistentDSU` と同じである。

## 注意点

- `base_query == -1` は初期状態を表す。それ以外は `[0, processed_queries())` でなければならない。
- 頂点は $[0, n)$、操作数は `MAX_QUERY` 以下でなければならない。
- 範囲違反、操作容量の超過、内部の永続領域の容量超過では `std::runtime_error` を送出する。
- 失敗した操作は操作数を増やさない。
- コピーは禁止されており、ムーブはできる。
