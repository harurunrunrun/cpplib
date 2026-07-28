---
title: Level Ancestor (レベル祖先)
documentation_of: ../../../../src/algorithm/tree/query/level_ancestor.hpp
---

根付き木で指定頂点の祖先を取得する二分累乗表。

## API

- `LevelAncestor(graph, root)`: 根付き木を前処理する。
- `size()`: 頂点数を返す。
- `depth(v)`: 根からの深さを返す。
- `kth_ancestor(v, distance)`: `distance` 本上の祖先を返す。存在しなければ `-1`。
- `ancestor_at_depth(v, target_depth)`: 深さを指定して祖先を返す。

## 時間計算量

- 構築: $O(N\log N)$。
- `size`, `depth`: $O(1)$。
- `kth_ancestor`, `ancestor_at_depth`: $O(\log N)$。

## 注意点

- 入力は連結な無向木とする。頂点範囲違反では例外を送出する。
