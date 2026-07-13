---
title: Partially Persistent Segment Tree Beats
documentation_of: ../src/structure/segtree/partially_persistent_segtree_beats.hpp
---

部分永続 Segment Tree Beats。更新は最新versionだけに行い、各tree indexの変更履歴から過去versionを読む。完全永続版には依存しない。

```cpp
PartiallyPersistentSegmentTreeBeats<T, MAX_SIZE, MAX_VERSIONS, MAX_CHANGES>
```

`MAX_VERSIONS` はversion 0を除く更新回数の上限。`MAX_CHANGES` は全node変更履歴の合計上限。

```cpp
int range_chmin(int l, int r, const T& x)
int range_chmax(int l, int r, const T& x)
int range_add(int l, int r, const T& x)
int set(int k, const T& x)
```

新しいversion番号を返す。容量不足時は変更履歴とversion数を更新前へ戻す。

```cpp
T get(int version, int k) const
T range_sum(int version, int l, int r) const
T range_min(int version, int l, int r) const
T range_max(int version, int l, int r) const
T all_sum(int version) const
T all_min(int version) const
T all_max(int version) const
```

versionを省略すると最新versionを読む。過去queryでは祖先nodeの加算・上限制約・下限制約を子へ順に合成する。

```cpp
int size() const
int versions() const
int latest_version() const
int changes_used() const
```

# 計算量

- constructor: $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- `range_chmin`, `range_chmax`: amortized $O(\log^2 N)$
- `range_add`, `set`: $O(\log N)$
- 最新versionの`get`, `range_sum`, `range_min`, `range_max`: $O(\log N)$
- 過去versionの同query: $O(\log N\log \mathtt{MAX\_CHANGES})$
- 最新versionの`all_*`: $O(1)$、過去versionの`all_*`: $O(\log \mathtt{MAX\_CHANGES})$
- version・change情報: $O(1)$

空間計算量は $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$。

# 共通API契約

version 0が初期状態で、queryは `0 <= version < versions()` の任意versionを参照できる。
更新は `latest_version()` だけを元に行い、新しいversion番号を返す。過去versionを更新元に
指定すると `runtime_error`。点は列内、区間は半開区間 `[l,r)` である。
境界探索のpredicateは単調かつ単位元に対してtrueが必要。

不正なversion・点・区間・predicate、version/node/change容量超過では `runtime_error`。
失敗した更新はversion・node・履歴使用量を更新前へ戻す。各APIの計算量は上記表の通り。
