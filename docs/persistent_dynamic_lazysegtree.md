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

更新は指定versionから分岐し、新しいversion番号を返す。

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `set`, 1点`apply`, 区間`apply`, `get`, `prod`: $O(\log \mathtt{MAX\_SIZE})$
- `max_right`, `min_left`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`, `all_prod`, `size`, `versions`, `latest_version`, `nodes_used`: $O(1)$

各更新が複製するノード数は $O(\log \mathtt{MAX\_SIZE})$。
