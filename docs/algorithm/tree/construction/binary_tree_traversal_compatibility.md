---
title: Binary Tree Traversal Compatibility (二分木巡回順整合判定) [TREEORD]
documentation_of: ../../../../src/algorithm/tree/construction/binary_tree_traversal_compatibility.hpp
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

## 容量を省略する可変長版

容量テンプレート引数をすべて省略すると、実行時サイズに合わせた `std::vector` を使う。容量を明示する固定長版も同じヘッダにある。

```cpp
bool result = binary_tree_traversal_compatibility(preorder, postorder, inorder);
bool fixed = binary_tree_traversal_compatibility<100000>(preorder, postorder, inorder);
```

$N$ 頂点に対して 期待 $O(N)$ 時間・$O(N)$ 空間（ハッシュ衝突時は最悪 $O(N^2)$ 時間）。
