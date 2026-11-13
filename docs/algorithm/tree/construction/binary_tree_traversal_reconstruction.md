---
title: Binary Tree Traversal Reconstruction (二分木の巡回順復元)
documentation_of: ../../../../src/algorithm/tree/construction/binary_tree_traversal_reconstruction.hpp
---

相異なるキーの preorder・inorder・postorder が同じ二分木の巡回順であるかを判定し、整合する場合は木を復元する。

## `BinaryTreeTraversalResult<MAX_SIZE>`

復元結果を表す。頂点番号には inorder 内の位置を使い、頂点 `v` のキーは `inorder[v]` である。

- `valid`: 3 つの列が整合するとき `true`。
- `node_count`: 3 つの列の長さが等しい場合はその長さ。それ以外は `0`。
- `root`: 根の頂点番号。空木では `-1`。
- `left_child[v]`: `v` の左の子。存在しなければ `-1`。
- `right_child[v]`: `v` の右の子。存在しなければ `-1`。
- デフォルトコンストラクタ: `valid == false`、`node_count == 0`、`root == -1` の結果を作る。
- コピーコンストラクタ・コピー代入: メタデータと `[0, node_count)` の子配列を複製する。
- ムーブコンストラクタ・ムーブ代入: コピーと同じく内容を複製する。移動元は変更しない。
- `size()`: `node_count` を返す。
- `empty()`: `node_count == 0` を返す。
- `explicit operator bool()`: `valid` を返す。

`root`・`left_child`・`right_child` は `valid == true` の場合にだけ参照する。`valid == false` の場合の値は規定しない。

- デフォルトコンストラクタ・`size()`・`empty()`・`operator bool()` の時間計算量: $O(1)$
- コピー・ムーブのコンストラクタと代入の時間計算量: $O(N)$（$N$ は `node_count`）
- メンバ参照の時間計算量: $O(1)$

`MAX_SIZE` を明示すると、子配列は容量 `MAX_SIZE` の固定長配列になる。省略する（`-1` にする）と、子配列は実際のサイズの `std::vector` になる。

## `reconstruct_binary_tree_traversals<MAX_SIZE>(preorder, inorder, postorder, hash = Hash(), equal = KeyEqual())`

3 つの巡回順を検査する。整合する場合は `valid == true` の復元結果を返す。

`preorder` と `inorder` から木を一意に復元し、得られた木の postorder と `postorder` を比較する。
処理はすべて反復的で、深さ $N$ の木でも再帰を使わない。

```cpp
auto result = reconstruct_binary_tree_traversals(preorder, inorder, postorder);
auto fixed = reconstruct_binary_tree_traversals<100000>(preorder, inorder, postorder);
```

- 時間計算量: 期待 $O(N)$、ハッシュテーブルの最悪時 $O(N^2)$
- 空間計算量: 可変長版は $O(N)$、固定容量版は $O(N + \texttt{MAX_SIZE})$

## `binary_tree_traversals_compatible<MAX_SIZE>(preorder, inorder, postorder, hash = Hash(), equal = KeyEqual())`

3 つの巡回順が整合するかを返す。

- 時間計算量: 期待 $O(N)$、ハッシュテーブルの最悪時 $O(N^2)$
- 空間計算量: 可変長版は $O(N)$、固定容量版は $O(N + \texttt{MAX_SIZE})$

## 注意点

- 3 つの列のキーはそれぞれ重複せず、要素集合が一致しなければならない。長さ・重複・要素集合・巡回順の不整合は `valid == false` として返す。空の 3 列は空木として整合する。
- いずれかの列の長さが `MAX_SIZE` を超える場合は `std::runtime_error` を送出する。
- `Hash` と `KeyEqual` は `std::unordered_map` の要件を満たし、互いに整合していなければならない。
