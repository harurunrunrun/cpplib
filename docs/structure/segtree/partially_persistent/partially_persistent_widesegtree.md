---
title: Partially Persistent Wide Segtree (部分永続ワイドセグメント木)
documentation_of: ../../../../src/structure/segtree/partially_persistent/partially_persistent_widesegtree.hpp
---

`long long` の和を扱う部分永続な疎セグメント木。`PartiallyPersistentDynamicSegtree` に和のモノイドを与えたラッパーである。更新できるのは最新 version だけで、過去の version は読み取り専用である。

```cpp
PartiallyPersistentWideSegtree<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>
```

別名 `PartiallyPersistentWide_Segtree` も定義されている。

容量（4 つのテンプレート引数）をすべて省略すると可変長版になり、`PartiallyPersistentWideSegtree<> tree(n);` のように長さを実行時に与える。固定容量版はデフォルトコンストラクタで構築し、範囲は $[0, \texttt{MAX_SIZE})$ になる。

## API

```cpp
int set(long long index, long long value)
int add(long long index, long long value)
long long get(int version, long long index) const
long long prod(int version, long long l, long long r) const
long long all_prod(int version) const
```

- `set`, `add`: 最新 version の 1 点に代入・加算した新しい version を作り、その番号を返す。
- `get`, `prod`, `all_prod`: 指定 version の値・半開区間 $[l, r)$ の和・全体の和を返す。`version` を省略すると最新 version を参照する。

状態取得用に次の関数がある。順に version 数、最新 version 番号、使用ノード数、変更履歴数を返し、すべて $O(1)$ である。

```cpp
int versions() const
int latest_version() const
int nodes_used() const
int changes_used() const
```

## 時間計算量

$N = \texttt{MAX_SIZE}$、$C = \texttt{MAX_CHANGES}$ とする。

- コンストラクタ: $O(\texttt{MAX_NODES}+C\log C)$
- `set`, `add`: $O(\log N\log C)$
- 最新 version の `get`, `prod`: $O(\log N)$
- 過去 version の `get`: $O(\log N+\log C)$
- 過去 version の `prod`: $O(\log N\log C)$
- 最新 version の `all_prod`: $O(1)$、過去 version の `all_prod`: $O(\log C)$
- 状態取得関数: $O(1)$

## 注意点

- version 0 が初期状態（全要素 0）で、クエリは `0 <= version < versions()` の任意の version を参照できる。
- 更新は常に `latest_version()` を元に行い、新しい version 番号を返す。
- 点は範囲内、区間は半開区間 $[l, r)$ である。
- 不正な version・点・区間、version・ノード・履歴の容量超過では `std::runtime_error` を送出する。
- 失敗した更新は version・ノード・履歴の使用量を更新前に戻す。
