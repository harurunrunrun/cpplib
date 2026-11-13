---
title: Tree Path Assign Maximum Subarray (木上パス代入・最大部分和) [GSS7]
documentation_of: ../../../../src/structure/tree/query/tree_path_assign_max_subarray.hpp
---

頂点値付きの森で、パス上の頂点値の一括代入と、指定した向きのパスを列とみなしたときの非空最大部分和の取得を扱う。
内部では `LazyLinkCutTree` を使う。

## API

### `TreePathAssignMaxSubarray<Value, MAX_SIZE>(values)`

初期頂点値 `values` から孤立頂点の森を作る。`MAX_SIZE` は省略でき、デフォルト値 `-1` では可変長版になる。時間計算量は固定長版で $O(\texttt{MAX_SIZE})$、可変長版で $O(n)$ である。

### `add_edge(left, right)`

異なる木に属する 2 頂点を辺で結んで `true` を返す。既に連結なら何もせず `false` を返す。時間計算量はならし $O(\log n)$ である。

### `path_max_subarray(left, right)`

`left` から `right` へのパス上の頂点値の列について、非空最大部分和を返す。時間計算量はならし $O(\log n)$ である。

### `assign_path(left, right, value)`

パス上の全頂点の値を `value` に代入する。時間計算量はならし $O(\log n)$ である。

## 注意点

- 頂点数は `MAX_SIZE` 以下でなければならない。範囲外の頂点では `runtime_error` を送出する。
- クエリの両端は連結でなければならない。実装は連結性を検査しない。
- 最大部分列は非空であり、全要素が負なら最大の 1 要素を返す。
- 和と代入結果は `Value` の範囲内でなければならない。
- 容量を指定した固定配列版（`TreePathAssignMaxSubarray<Value, N>`）と可変長版（`TreePathAssignMaxSubarray<Value>`）は同じヘッダで利用できる。
