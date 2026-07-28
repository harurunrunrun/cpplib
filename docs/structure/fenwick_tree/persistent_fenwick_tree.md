---
title: Persistent Fenwick Tree (永続Fenwick木)
documentation_of: ../../../src/structure/fenwick_tree/persistent_fenwick_tree.hpp
---

更新を時系列の版として保持する部分永続Fenwick木。

## API

- `PersistentFenwickTree(n)`: 零列を版0として構築する。
- `size()`, `version_count()`: 列長、版数を返す。
- `add(position,delta)`: 最新版へ加算し、新しい版番号を返す。
- `prefix_sum(version,right)`, `sum(version,left,right)`: 指定版の和を返す。

## 時間計算量

- 構築: $O(N)$。
- `add`: $O(\log N)$。
- `prefix_sum`, `sum`: $O(\log^2 N)$。
- `size`, `version_count`: $O(1)$。

## 注意点

- 過去版から分岐する更新は扱わず、更新は常に最新系列へ追加される。
