---
title: Partially Persistent Wide Segtree
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

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_CHANGES}\log \mathtt{MAX\_CHANGES})$
- `set`, `add`: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- 最新versionの`get`, `prod`: $O(\log \mathtt{MAX\_SIZE})$
- 過去versionの`get`: $O(\log \mathtt{MAX\_SIZE}+\log \mathtt{MAX\_CHANGES})$
- 過去versionの`prod`: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- 最新versionの`all_prod`: $O(1)$、過去versionの`all_prod`: $O(\log \mathtt{MAX\_CHANGES})$
- version・node・change情報: $O(1)$
