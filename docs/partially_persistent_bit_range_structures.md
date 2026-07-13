---
title: Partially Persistent BitAssign RangeSum RangeFlip (部分永続ビット代入・区間和・区間反転)
documentation_of: ../src/structure/segtree/partially_persistent_bitassign_rangesum_rangeflip.hpp
---

$0,1$ 列の区間更新と区間和を部分永続化する。更新は最新versionだけに行い、過去の全versionを参照できる。

完全永続版は使用せず、各疎segment tree nodeの変更履歴だけを保存する。

```cpp
PartiallyPersistentBitAssignRangeSumRangeFlip<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>
```

`PartiallyPersistentDynamicBitAssignRangeSumRangeFlip` という同じAPIの別名も定義される。

# 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- `assign`, `flip`, `set_one`, `set`: $O(\log \mathtt{MAX\_SIZE})$
- 最新versionの`sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- 過去versionの`sum`, `get`: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- `fork`, `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`: $O(1)$

空間計算量は $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$。

## 注意点

```cpp
int assign(int version, long long l, long long r, bool value)
int flip(int version, long long l, long long r)
int set_one(int version, long long l, long long r)
long long sum(int version, long long l, long long r) const
bool get(int version, long long k) const
int fork(int version)
long long size() const
int versions() const
int latest_version() const
int nodes_used() const
int changes_used() const
```

version省略updateは最新版を使う。更新元は `latest_version()` に限り、新version番号を返す。
queryは任意の有効versionを参照する。`set` は `set_one` と同じ。
区間は半開区間。有効version・範囲・最新版条件・各容量の違反では `runtime_error`。
失敗した更新は使用量を元へ戻す。各APIの計算量は下表の通り。
