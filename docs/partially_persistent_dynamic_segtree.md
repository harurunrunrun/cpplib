---
title: Partially Persistent Dynamic Segment Tree
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

更新と過去versionの区間演算は $O(\log MAX\_SIZE \log MAX\_CHANGES)$。1点取得は $O(\log MAX\_SIZE + \log MAX\_CHANGES)$。
