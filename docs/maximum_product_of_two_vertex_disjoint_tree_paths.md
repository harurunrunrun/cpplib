---
title: Maximum Product of Two Vertex-Disjoint Tree Paths (頂点素な2本の木上パスの長さ積最大化) [TWOPATHS]
documentation_of: ../src/algorithm/tree/dynamic_programming/maximum_product_of_two_vertex_disjoint_tree_paths.hpp
---

無重み木から頂点を共有しない2本の単純パスを選び、辺数で測った長さの積を最大化する。
最大値に加えて、その値を証明する切断辺と両成分の直径を返す。

## API

### `TreeTwoDisjointPathsResult`

    struct TreeTwoDisjointPathsResult {
        long long product;
        int cut_edge_index;
        int cut_u;
        int cut_v;
        int first_diameter;
        int second_diameter;
    };

`product` は最大積。
`cut_edge_index` は最大値を与える入力辺の最小IDで、
`cut_u` と `cut_v` はその辺の入力順の両端である。
`first_diameter` と `second_diameter` は、切断後にそれぞれ
`cut_u` と `cut_v` を含む成分の直径である。
各fieldの参照は $O(1)$。

### `maximum_product_of_two_vertex_disjoint_tree_paths<MAX_SIZE>(vertex_count, edges)`

`edges` が表す無重み木について最大積と証拠を返す。
任意の頂点素な2パスを隔てる辺を切ると各パスは別成分に入り、
各成分では直径が最長パスなので、全切断辺の両側の直径積を比較できる。

## 時間計算量

`N = vertex_count` とする。

- 関数全体: $O(N)$
- 返却値の各field参照: $O(1)$

使用メモリは固定容量workspaceと長さ $N-1$ の内部列を合わせて
$O(MAX\_SIZE + N)$。再帰は使用しない。

## 注意点

- `MAX_SIZE > 0` かつ `0 < vertex_count <= MAX_SIZE` が必要。
- `edges` はちょうど `vertex_count - 1` 本で、頂点番号は
  `[0, vertex_count)`、グラフは連結かつ閉路なしでなければならない。
- 自己辺、多重辺、閉路、非連結、辺数不一致、範囲外頂点、容量超過では
  `runtime_error` を送出する。
- `vertex_count == 1` では `product == 0`、3つの辺fieldは `-1`、
  両直径は0となる。
- 長さ0の1頂点パスを許す。積は `long long` で返す。
