---
title: Rollback BitAssign RangeSum RangeFlip (ロールバック可能ビット代入・区間和・区間反転)
documentation_of: ../src/structure/segtree/rollback_bitassign_rangesum_rangeflip.hpp
---

$0,1$ 列の区間更新と区間和をrollback可能にする。変更前nodeを履歴へ保存し、指定snapshot以後の更新を破棄する。

```cpp
RollbackBitAssignRangeSumRangeFlip<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>

int snapshot()
void rollback(int version)
```

`RollbackDynamicBitAssignRangeSumRangeFlip` という同じAPIの別名も定義される。

# 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- `assign`, `flip`, `set`, `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`, `snapshot`, `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`: $O(1)$
- `rollback`: 巻き戻す変更記録数を $K$ として $O(K)$

## 注意点

```cpp
int assign(long long l, long long r, bool value)
int flip(long long l, long long r)
int set_one(long long l, long long r)
int set(long long l, long long r)
long long sum(long long l, long long r) const
bool get(long long k) const
int fork()
int snapshot() const
void rollback(int snapshot)
long long size() const
int versions() const
int latest_version() const
int nodes_used() const
int changes_used() const
```

各更新と `fork` は新しい状態番号を返す。`snapshot` は現在番号、`rollback` は指定状態より
後を破棄する。区間は半開区間。`set/set_one` は1上書きである。
範囲・snapshot・node/version/change容量違反は `runtime_error`。
失敗した更新は本体と履歴を元へ戻す。各APIの計算量は下表の通り。
