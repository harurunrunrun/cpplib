---
title: Partially Persistent Dynamic Segment Tree (部分永続動的セグメント木)
documentation_of: ../../../../src/structure/segtree/partially_persistent/partially_persistent_dynamic_segtree.hpp
---

巨大な添字範囲を扱う部分永続セグメント木。更新は最新 version に対してだけ行い、各ノードの変更履歴から過去の version を参照する。

```cpp
template<auto Monoid, long long MAX_SIZE = -1, int MAX_NODES = -1, int MAX_VERSIONS = -1, int MAX_CHANGES = -1>
struct PartiallyPersistentDynamicSegtree;
```

`Monoid` は `S`, `op`, `e` を持つモノイドのオブジェクトである。

- `MAX_SIZE`: 添字範囲の長さ（固定容量版では範囲は $[0, \texttt{MAX_SIZE})$ に固定）
- `MAX_NODES`: 作成できる疎ノード数
- `MAX_VERSIONS`: version 0 を除く更新回数の上限
- `MAX_CHANGES`: 全ノードの変更履歴数の上限

容量（4 つのテンプレート引数）をすべて省略すると可変長版になり、`PartiallyPersistentDynamicSegtree<Monoid> tree(n);` のように長さを実行時に与える。ノード・version・履歴は使用量に応じて `std::vector` を拡張する。

## API

```cpp
PartiallyPersistentDynamicSegtree()          // 固定容量版
PartiallyPersistentDynamicSegtree(long long n = 0)  // 可変長版
int set(long long index, S value)
S get(int version, long long index) const
S prod(int version, long long l, long long r) const
S all_prod(int version) const
long long max_right(int version, long long l, F f, Args... args) const
long long min_left(int version, long long r, F f, Args... args) const
```

- コンストラクタ: 全要素 `e()` の状態を version 0 として作る。
- `set`: 最新 version の 1 点を書き換えた新しい version を作り、その番号を返す。
- `get`, `prod`, `all_prod`, `max_right`, `min_left`: 指定 version を参照する。`version` を省略すると最新 version を参照する。
- `max_right`, `min_left` の述語は追加引数 `args...` を受け取れる。

状態取得用に次の関数がある。順に version 数、最新 version 番号、使用ノード数、変更履歴数を返す。

```cpp
int versions() const
int latest_version() const
int nodes_used() const
int changes_used() const
```

## 時間計算量

$N = \texttt{MAX_SIZE}$（可変長版では $n$）、$C = \texttt{MAX_CHANGES}$ とする。

- コンストラクタ: 固定容量版は $O(\texttt{MAX_NODES}+C\log C)$
- `set`: $O(\log N\log C)$
- 最新 version の `get`, `prod`, `max_right`, `min_left`: $O(\log N)$
- 過去 version の `get`: $O(\log N+\log C)$
- 過去 version の `prod`, `max_right`, `min_left`: $O(\log N\log C)$
- 最新 version の `all_prod`: $O(1)$、過去 version の `all_prod`: $O(\log C)$
- 状態取得関数: $O(1)$

過去 version の参照は各ノードの変更履歴上のダブリングで行うため、空間計算量は固定容量版で $O(\texttt{MAX_NODES}+C\log C)$ である。

## 注意点

- version 0 が初期状態で、クエリは `0 <= version < versions()` の任意の version を参照できる。
- 更新は常に `latest_version()` を元に行い、新しい version 番号を返す。過去の version を更新元にする API はない。
- 点は範囲内、区間は半開区間 $[l, r)$ である。
- 境界探索の述語は単調で、単位元に対して `true` を返す必要がある。
- 不正な version・点・区間、述語が単位元で `false`、version・ノード・履歴の容量超過では `std::runtime_error` を送出する。
- 失敗した更新は version・ノード・履歴の使用量を更新前に戻す。
- コピーコンストラクタとコピー代入は削除されている。
