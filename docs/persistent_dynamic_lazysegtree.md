---
title: Persistent Dynamic Lazy Segment Tree
documentation_of: ../src/structure/segtree/persistent_dynamic_lazysegtree.hpp
---

巨大な添字範囲を扱う完全永続遅延セグメント木。

```cpp
PersistentDynamicLazySegtree<MonoidActLen, MAX_SIZE, MAX_NODES, MAX_VERSIONS>
```

`MAX_NODES`は全versionで共有するノード数、`MAX_VERSIONS`はversion 0を除くversion数の上限。

```cpp
int set(int version, long long index, S value)
int apply(int version, long long index, T action)
int apply(int version, long long l, long long r, T action)
int fork(int version)
S get(int version, long long index)
S prod(int version, long long l, long long r)
S all_prod(int version)
long long max_right(int version, long long l, F f)
long long min_left(int version, long long r, F f)
```

更新は指定versionから分岐し、新しいversion番号を返す。更新・区間取得は $O(\log MAX\_SIZE)$。
