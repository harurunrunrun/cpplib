---
title: Binary Tree Traversal Compatibility (二分木巡回順整合判定) [TREEORD]
documentation_of: ../src/algorithm/tree/binary_tree_traversal_compatibility.hpp
---

preorder・postorder・inorderが同じ二分木を表せるかを判定する。

## API

### `binary_tree_traversal_compatibility<MAX_SIZE>(preorder, postorder, inorder)`

3列が整合するなら `true` を返す。

- 時間計算量: $O(N)$
- 追加空間計算量: $O(N)$

## 注意点

- 3列の長さは同じで `MAX_SIZE` 以下、keyは相異なる必要がある。
- 長さ不一致・重複・集合不一致・容量超過では `runtime_error`。
