---
title: Binary Tree Traversal Compatibility (二分木巡回順整合判定) [TREEORD]
documentation_of: ../../../../src/algorithm/tree/construction/binary_tree_traversal_compatibility.hpp
---

preorder・postorder・inorder の 3 つの列が、同じ二分木の巡回順を表せるかを判定する。

## API

```cpp
template<int MAX_SIZE = -1>
bool binary_tree_traversal_compatibility(
    const std::vector<int>& preorder,
    const std::vector<int>& postorder,
    const std::vector<int>& inorder
)
```

3 つの列が整合するなら `true` を返す。引数の順序は preorder, postorder, inorder である。
内部では `binary_tree_traversals_compatible<MAX_SIZE>(preorder, inorder, postorder)` を呼ぶ。

`MAX_SIZE` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` を使う。明示すると固定容量の配列を使う。

```cpp
bool result = binary_tree_traversal_compatibility(preorder, postorder, inorder);
bool fixed = binary_tree_traversal_compatibility<100000>(preorder, postorder, inorder);
```

## 時間計算量

$N$ を列の長さとする。期待 $O(N)$ 時間（ハッシュ衝突時は最悪 $O(N^2)$ 時間）、追加空間 $O(N)$（固定容量版では $O(N + \texttt{MAX_SIZE})$）である。

## 注意点

- キーは各列の中で相異なる必要がある。
- 長さの不一致・重複・要素集合の不一致・巡回順の不整合では `false` を返す。
- いずれかの列の長さが `MAX_SIZE` を超える場合は `std::runtime_error` を送出する。
