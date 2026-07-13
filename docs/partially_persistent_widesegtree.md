---
title: Partially Persistent Wide Segtree (部分永続ワイドセグメント木)
documentation_of: ../src/structure/segtree/partially_persistent_widesegtree.hpp
---

`long long`の和を扱う部分永続な疎セグメント木。更新対象は最新versionだけで、過去versionは読み取り専用。

```cpp
PartiallyPersistentWideSegtree<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>
```

```cpp
int set(long long index, long long value)
int add(long long index, long long value)
long long get(int version, long long index)
long long prod(int version, long long l, long long r)
long long all_prod(int version)
```

# 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_CHANGES}\log \mathtt{MAX\_CHANGES})$
- `set`, `add`: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- 最新versionの`get`, `prod`: $O(\log \mathtt{MAX\_SIZE})$
- 過去versionの`get`: $O(\log \mathtt{MAX\_SIZE}+\log \mathtt{MAX\_CHANGES})$
- 過去versionの`prod`: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
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

順にversion数、最新番号、使用node数、変更履歴数を返し、すべて (O(1))。
