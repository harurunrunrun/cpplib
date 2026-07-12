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

`range_chmin` と `range_chmax` はamortized $O(\log^2 N)$。最新更新は通常版と同じ計算量。過去nodeの取得は $O(\log\mathtt{MAX\_CHANGES})$ のため、区間queryは $O(\log N\log\mathtt{MAX\_CHANGES})$。空間計算量は $O(N+\mathtt{MAX\_CHANGES})$。
