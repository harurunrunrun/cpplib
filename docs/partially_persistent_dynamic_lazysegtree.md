---
title: Partially Persistent Dynamic Lazy Segment Tree
documentation_of: ../src/structure/segtree/partially_persistent_dynamic_lazysegtree.hpp
---

巨大な添字範囲を扱う部分永続遅延セグメント木。更新は最新versionにだけ行い、ノードの値と遅延作用の履歴から過去versionを参照する。

```cpp
PartiallyPersistentDynamicLazySegtree<MonoidActLen, MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>
```

```cpp
int set(long long index, S value)
int apply(long long index, T action)
int apply(long long l, long long r, T action)
S get(int version, long long index)
S prod(int version, long long l, long long r)
S all_prod(int version)
long long max_right(int version, long long l, F f)
long long min_left(int version, long long r, F f)
```

`MAX_NODES`は疎ノード数、`MAX_CHANGES`は値と遅延作用をまとめた変更履歴数の上限。更新・区間取得は $O(\log MAX\_SIZE \log MAX\_CHANGES)$。
