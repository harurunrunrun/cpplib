---
title: Persistent Bit Range Structures
documentation_of: ../src/structure/segtree/persistent_bitassign_rangesum_rangeflip.hpp
---

$0,1$ 列の区間更新と区間和を、path copyした暗黙segment treeで完全永続化する。

```cpp
PersistentBitAssignRangeSum<MAX_SIZE, MAX_NODES, MAX_VERSIONS>
PersistentBitAssignRangeSumRangeFlip<MAX_SIZE, MAX_NODES, MAX_VERSIONS>
PersistentBitOverwriteRangeSum<MAX_SIZE, MAX_NODES, MAX_VERSIONS>
```

`PersistentDynamic...` という同じAPIの別名も定義される。完全永続版は元の密・疎実装のどちらに対しても疎なpath-copy表現を使う。

```cpp
int assign(int version, long long l, long long r, bool value)
int flip(int version, long long l, long long r)
int set(int version, long long l, long long r)
int fork(int version)
long long sum(int version, long long l, long long r)
bool get(int version, long long position)
```

更新は新しいversionを返す。`MAX_VERSIONS` はversion 0を含む総version数。

## 計算量

- update: $O(\log MAX\_SIZE)$
- query: $O(\log MAX\_SIZE)$
