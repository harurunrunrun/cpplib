---
title: Partially Persistent Dynamic Segment Tree (部分永続動的セグメント木)
documentation_of: ../src/structure/segtree/partially_persistent_dynamic_segtree.hpp
---

巨大な添字範囲を扱う部分永続セグメント木。更新は最新versionにだけ行い、各ノードの変更履歴から過去versionを参照する。

```cpp
PartiallyPersistentDynamicSegtree<Monoid, MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>
```

- `MAX_NODES`: 作成できる疎ノード数
- `MAX_VERSIONS`: version 0を除く更新回数
- `MAX_CHANGES`: 全ノードの変更履歴数

```cpp
int set(long long index, S value)
S get(int version, long long index)
S prod(int version, long long l, long long r)
S all_prod(int version)
long long max_right(int version, long long l, F f)
long long min_left(int version, long long r, F f)
```

# 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_CHANGES}\log \mathtt{MAX\_CHANGES})$
- `set`: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- 最新versionの`get`, `prod`, `max_right`, `min_left`: $O(\log \mathtt{MAX\_SIZE})$
- 過去versionの`get`: $O(\log \mathtt{MAX\_SIZE}+\log \mathtt{MAX\_CHANGES})$
- 過去versionの`prod`, `max_right`, `min_left`: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- 最新versionの`all_prod`: $O(1)$、過去versionの`all_prod`: $O(\log \mathtt{MAX\_CHANGES})$
- version・node・change情報: $O(1)$

## 注意点

version 0が初期状態で、queryは `0 <= version < versions()` の任意versionを参照できる。
更新は `latest_version()` だけを元に行い、新しいversion番号を返す。過去versionを更新元に
指定すると `runtime_error`。点は列内、区間は半開区間 `[l,r)` である。
境界探索のpredicateは単調かつ単位元に対してtrueが必要。

不正なversion・点・区間・predicate、version/node/change容量超過では `runtime_error`。
失敗した更新はversion・node・履歴使用量を更新前へ戻す。各APIの計算量は上記表の通り。

# 状態API

```cpp
int versions() const
int latest_version() const
int nodes_used() const
int changes_used() const
```

順にversion数、最新version番号、使用node数、変更履歴数を返し、すべて (O(1))。
