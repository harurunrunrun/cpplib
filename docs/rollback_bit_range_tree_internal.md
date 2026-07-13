---
title: Rollback Bit Range Tree Internal (ロールバック可能ビット区間木内部実装)
documentation_of: ../src/structure/segtree/detail/rollback_bit_range_tree.hpp
---

bit列の区間代入・反転・区間和とrollbackを扱う動的区間木の共通実装。

## API

```cpp
bit_range_tree_detail::RollbackBitRangeTree<
    MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES
> tree(n);
```

- `size()`：列長を返す。
- `versions()` / `latest_version()` / `snapshot()`：version情報を返す。
- `nodes_used()` / `changes_used()`：使用中のnode数と変更履歴数を返す。
- `fork()`：現在状態と同じ新versionを作る。
- `assign(l,r,value)`：`[l,r)` を0または1へ代入する。
- `flip(l,r)`：`[l,r)` を反転する。
- `set_one(l,r)` / `set(l,r)`：`[l,r)` を1にする。
- `sum(l,r)`：現在の `[l,r)` にある1の個数を返す。
- `get(k)`：現在の `k` 番目を返す。
- `rollback(version)`：指定versionの状態へ戻し、それより新しいversionを破棄する。

更新操作は更新後のversion番号を返す。

## 時間計算量

- constructor：$O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- 更新、`sum`, `get`：$O(\log \mathtt{MAX\_SIZE})$
- `fork`, `snapshot`と使用量取得：$O(1)$
- `rollback`：取り消す変更履歴数を $K$ として $O(K)$

更新1回の追加空間計算量は $O(\log \mathtt{MAX\_SIZE})$。

## 注意点

- 公開ラッパー群の内部部品であり、通常は直接使用しない。
- rollback後に破棄されたversion番号は無効になる。
- `0 < n <= MAX_SIZE`、区間は `0 <= l <= r <= n`、点は `[0,n)`。範囲、version、node・version・履歴容量の違反では `runtime_error`。
