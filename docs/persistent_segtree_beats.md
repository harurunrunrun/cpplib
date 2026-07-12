---
title: Persistent Segment Tree Beats
documentation_of: ../src/structure/segtree/persistent_segtree_beats.hpp
---

完全永続 Segment Tree Beats。更新で変更するnodeだけをpath copyし、任意versionから分岐できる。

```cpp
PersistentSegmentTreeBeats<T, MAX_SIZE, MAX_NODES, MAX_VERSIONS>
```

`MAX_NODES` は初期木と全更新で作るnode数の合計上限。`MAX_VERSIONS` はversion 0を除いて追加できるversion数の上限。

```cpp
int range_chmin(int version, int l, int r, const T& x)
int range_chmax(int version, int l, int r, const T& x)
int range_add(int version, int l, int r, const T& x)
int set(int version, int k, const T& x)
int fork(int version)
```

新しいversion番号を返す。容量不足時は例外を送出し、node数とversion数を更新前へ戻す。

```cpp
T get(int version, int k) const
T range_sum(int version, int l, int r) const
T range_min(int version, int l, int r) const
T range_max(int version, int l, int r) const
T all_sum(int version) const
T all_min(int version) const
T all_max(int version) const
```

queryは木を書き換えない。祖先nodeに残る加算・上限制約・下限制約を子へ順に合成して評価する。

```cpp
int size() const
int versions() const
int latest_version() const
int nodes_used() const
```

区間は0-indexed半開区間。`range_chmin` と `range_chmax` はamortized $O(\log^2 N)$、その他の更新とqueryは $O(\log N)$、`all_*` は $O(1)$。空間計算量は $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$。
