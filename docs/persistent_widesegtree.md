---
title: Persistent Wide Segtree
documentation_of: ../src/structure/segtree/persistent_widesegtree.hpp
---

`long long`の和を扱う完全永続な疎セグメント木。

```cpp
PersistentWideSegtree<MAX_SIZE, MAX_NODES, MAX_VERSIONS>
```

```cpp
int set(int version, long long index, long long value)
int add(int version, long long index, long long value)
int fork(int version)
long long get(int version, long long index)
long long prod(int version, long long l, long long r)
long long all_prod(int version)
```

更新・取得は $O(\log MAX\_SIZE)$。`fork`と`all_prod`は $O(1)$。
