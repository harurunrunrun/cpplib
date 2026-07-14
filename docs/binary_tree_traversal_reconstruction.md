---
title: Binary Tree Traversal Reconstruction (二分木の巡回順復元) [TREEORD]
documentation_of: ../src/algorithm/tree/binary_tree_traversal_reconstruction.hpp
---

相異なるキーの preorder・inorder・postorder が同じ二分木の巡回順か判定し、整合する場合は木を復元する。

## `BinaryTreeTraversalResult<MAX_SIZE>`

復元結果。頂点番号には inorder 内の位置を使い、頂点 `v` のキーは `inorder[v]` である。

- `valid`: 3列が整合するとき `true`
- `node_count`: 3列の長さが等しい場合はその長さ。それ以外は `0`
- `root`: 根の頂点番号。空木では `-1`
- `left_child[v]`: `v` の左の子。存在しなければ `-1`
- `right_child[v]`: `v` の右の子。存在しなければ `-1`
- default constructor: `valid == false`、`node_count == 0`、`root == -1` の結果を作る
- copy constructor・copy assignment: metadataと `[0, node_count)` の子配列を複製する
- move constructor・move assignment: copyと同じ内容を複製する。移動元は変更しない
- `size()`: `node_count` を返す
- `empty()`: `node_count == 0` を返す
- `explicit operator bool()`: `valid` を返す

`root`・`left_child`・`right_child` は `valid == true` の場合だけ参照する。`valid == false` の場合の値は規定しない。

- default constructor・`size()`・`empty()`・`operator bool()` の時間計算量: $O(1)$
- copy/move constructor・assignmentの時間計算量: $O(\mathtt{node\_count})$
- field参照の時間計算量: $O(1)$

結果の容量はコンパイル時定数 `MAX_SIZE`。

## `reconstruct_binary_tree_traversals<MAX_SIZE>(preorder, inorder, postorder, hash = Hash(), equal = KeyEqual())`

3つの巡回順を検査する。整合する場合は `valid == true` の復元結果を返す。

`preorder` と `inorder` から木を一意に復元し、得られた木の postorder と `postorder` を比較する。処理はすべて反復的で、深さ $N$ の木でも再帰を使わない。

- 時間計算量: 期待 $O(N)$、hash tableの最悪時 $O(N^2)$
- 空間計算量: $O(N + \mathtt{MAX\_SIZE})$

## `binary_tree_traversals_compatible<MAX_SIZE>(preorder, inorder, postorder, hash = Hash(), equal = KeyEqual())`

3つの巡回順が整合するか返す。

- 時間計算量: 期待 $O(N)$、hash tableの最悪時 $O(N^2)$
- 空間計算量: $O(N + \mathtt{MAX\_SIZE})$

## 注意点

3列のキーはそれぞれ重複せず、要素集合が一致しなければならない。長さ・重複・要素集合・巡回順の不整合は `valid == false` で返す。空の3列は空木として整合する。

いずれかの列の長さが `MAX_SIZE` を超える場合は `runtime_error` を送出する。`Hash` と `KeyEqual` は `unordered_map` の要件を満たし、互いに整合していなければならない。
