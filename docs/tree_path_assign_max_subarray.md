---
title: Tree Path Assign Maximum Subarray (木上パス代入・最大部分和) [GSS7]
documentation_of: ../src/structure/tree/tree_path_assign_max_subarray.hpp
---

頂点値付き木のパス一括代入と、指定方向パス列の非空最大部分和を扱う。

## API

### `TreePathAssignMaxSubarray<Value, MAX_SIZE>(values)`

初期頂点値から孤立頂点の森を作る。時間計算量は $O(n)$。

### `add_edge(left, right)`

異なる木を結び、成功したかを返す。時間計算量は償却 $O(\log n)$。

### `path_max_subarray(left, right)`

パス列の最大部分和を返す。時間計算量は償却 $O(\log n)$。

### `assign_path(left, right, value)`

パス上の全頂点値を代入する。時間計算量は償却 $O(\log n)$。

## 注意点

- 頂点数は `MAX_SIZE` 以下で、query両端は連結でなければならない。
- 最大部分列は非空であり、全要素が負なら最大の1要素を返す。
- 和と代入結果は `Value` の範囲内でなければならない。
