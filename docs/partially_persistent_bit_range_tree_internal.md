---
title: Partially Persistent Bit Range Tree Internal (部分永続ビット区間木内部実装)
documentation_of: ../src/structure/segtree/detail/partially_persistent_bit_range_tree.hpp
---

bit列の区間代入・反転・区間和を扱う部分永続動的区間木の共通実装。

## API

```cpp
bit_range_tree_detail::PartiallyPersistentBitRangeTree<
    MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES
> tree(n);
```

- `size()`：列長を返す。
- `versions()` / `latest_version()`：version数と最新versionを返す。
- `nodes_used()` / `changes_used()`：使用中のnode数と変更履歴数を返す。
- `fork(version)`：最新版と同内容の新versionを作る。
- `assign(version,l,r,value)` / `assign(l,r,value)`：最新版の `[l,r)` を0または1へ代入した新versionを返す。
- `flip(version,l,r)` / `flip(l,r)`：最新版の `[l,r)` を反転した新versionを返す。
- `set_one`, `set`：最新版の `[l,r)` を1にした新versionを返す。
- `sum(version,l,r)`：指定versionの `[l,r)` にある1の個数を返す。
- `get(version,k)`：指定versionの `k` 番目を返す。

## 時間計算量

- constructor：$O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- 更新、最新版の`sum` / `get`：$O(\log \mathtt{MAX\_SIZE})$
- 過去versionの`sum` / `get`：$O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- `fork`と使用量取得：$O(1)$

更新1回の追加node・変更履歴は $O(\log \mathtt{MAX\_SIZE})$。

## 注意点

- 公開ラッパー群の内部部品であり、通常は直接使用しない。
- 更新元は常に `latest_version()` でなければならない。過去versionはqueryだけ可能。
- `0 < n <= MAX_SIZE`、区間は `0 <= l <= r <= n`、点は `[0,n)`。version、範囲、node・version・履歴容量の違反では `runtime_error`。
