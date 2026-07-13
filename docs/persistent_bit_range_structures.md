---
title: Persistent BitAssign RangeSum RangeFlip (完全永続ビット代入・区間和・区間反転)
documentation_of: ../src/structure/segtree/persistent_bitassign_rangesum_rangeflip.hpp
---

$0,1$ 列の区間更新と区間和を、path copyした暗黙segment treeで完全永続化する。

```cpp
PersistentBitAssignRangeSumRangeFlip<MAX_SIZE, MAX_NODES, MAX_VERSIONS>
```

`PersistentDynamicBitAssignRangeSumRangeFlip` という同じAPIの別名も定義される。
完全永続版は疎なpath-copy表現を使う。

```cpp
int assign(int version, long long l, long long r, bool value)
int flip(int version, long long l, long long r)
int set_one(int version, long long l, long long r)
int fork(int version)
long long sum(int version, long long l, long long r)
bool get(int version, long long position)
```

更新は新しいversionを返す。`MAX_VERSIONS` はversion 0を含む総version数。

## 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `assign`, `flip`, `set_one`: $O(\log \mathtt{MAX\_SIZE})$
- `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`, `size`, `versions`, `latest_version`, `nodes_used`: $O(1)$

## 注意点

```cpp
long long size() const
int versions() const
int latest_version() const
int nodes_used() const
int fork(int version)
int assign(int version, long long l, long long r, bool value)
int flip(int version, long long l, long long r)
int set_one(int version, long long l, long long r)
long long sum(int version, long long l, long long r) const
bool get(int version, long long k) const
```

`assign/flip/set_one` は指定versionの半開区間 `[l,r)` を更新した新versionを返す。
`fork` は内容を変えずに分岐する。`sum/get` は指定versionの1の個数・1点を返す。

`0 < n <= MAX_SIZE`、有効version、`0 <= l <= r <= n`、`0 <= k < n` が必要。
範囲・version・node/version容量違反は `runtime_error`。失敗時に使用量は増えない。
各APIの計算量は下表の通り。
