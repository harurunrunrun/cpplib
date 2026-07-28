---
title: Persistent Fenwick Tree (永続Fenwick木)
documentation_of: ../../../src/structure/fenwick_tree/persistent_fenwick_tree.hpp
---

任意の過去版から分岐して更新できる完全永続Fenwick木。

## API

- `PersistentFenwickTree(n)`: 零列を版0として構築する。
- `size()`, `version_count()`: 列長、版数を返す。
- `add(base_version,position,delta)`: 指定版へ加算した新しい版番号を返す。
- `add(position,delta)`: 最新版を基に加算する簡便API。
- `prefix_sum(version,right)`, `sum(version,left,right)`: 指定版の和を返す。

## 時間計算量

- 構築: $O(1)$。
- `add`: $O(\log^2 N)$、追加空間 $O(\log^2 N)$。
- `prefix_sum`, `sum`: $O(\log^2 N)$。
- `size`, `version_count`: $O(1)$。

## 注意点

- 更新後も基にした版を含む既存の全版は不変である。
- `T{}`、加算、減算が利用できる型を `T` に用いる。
- 負の長さは `std::invalid_argument`、`INT_MAX` は内部の1-based境界を
  表現できないため `std::length_error` を送出する。
- 不正な版・位置・区間は `std::out_of_range` を送出する。
