---
title: Fenwick Tree of Ordered Sets (順序集合Fenwick木)
documentation_of: ../../../src/structure/fenwick_tree/fenwick_ordered_sets.hpp
---

位置と値の二次元点集合を、値候補を事前圧縮したFenwick木で管理する。

## API

- `FenwickOrderedSets(n,possible_updates)`: 更新し得る `(position,value)` を登録する。
- `size()`: 列長を返す。
- `add(position,value,delta)`: 点の個数を増減する。
- `prefix_count_less(right,value)`: 位置 `[0,right)` で値未満を数える。
- `count_less(left,right,value)`: 指定位置区間で値未満を数える。
- `count(left,right,lower,upper)`: 値も半開区間に限定して数える。

## 時間計算量

- 構築: $O(K\log^2 N)$。
- 各更新・クエリ: $O(\log^2 N)$。
- `size`: $O(1)$。

## 注意点

- `add` する値は構築時に同じ位置の候補として登録しておく。
