---
title: Maximum Product of Two Vertex-Disjoint Tree Paths (頂点素な2本の木上パスの長さ積最大化) [TWOPATHS]
documentation_of: ../../../../src/algorithm/tree/dynamic_programming/maximum_product_of_two_vertex_disjoint_tree_paths.hpp
---

重みなし木から頂点を共有しない 2 本の単純パスを選び、辺数で測った長さの積を最大化する。
最大値に加えて、その値の根拠となる切断辺と、両側の成分の直径を返す。

## API

### `TreeTwoDisjointPathsResult`

```cpp
struct TreeTwoDisjointPathsResult{
    long long product;
    int cut_edge_index;
    int cut_u;
    int cut_v;
    int first_diameter;
    int second_diameter;
};
```

- `product` は最大積である。
- `cut_edge_index` は最大値を与える入力辺のうち最小の ID で、`cut_u` と `cut_v` はその辺の入力順の両端である。
- `first_diameter` と `second_diameter` は、切断後にそれぞれ `cut_u` と `cut_v` を含む成分の直径である。
- 各メンバの参照は $O(1)$ である。

### `maximum_product_of_two_vertex_disjoint_tree_paths<MAX_SIZE>(vertex_count, edges)`

```cpp
template<int MAX_SIZE = -1>
TreeTwoDisjointPathsResult maximum_product_of_two_vertex_disjoint_tree_paths(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
);
```

`edges` が表す重みなし木について、最大積とその根拠を返す。
頂点素な 2 本のパスを隔てる辺を切ると、各パスは別々の成分に入る。各成分では直径が最長パスなので、すべての切断辺について両側の直径の積を比較すればよい。
内部では `tree_edge_split_diameters<MAX_SIZE>` を使う。

`MAX_SIZE` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` を使う。明示すると固定容量の作業領域を使う。

```cpp
auto result = maximum_product_of_two_vertex_disjoint_tree_paths(vertex_count, edges);
auto fixed = maximum_product_of_two_vertex_disjoint_tree_paths<100000>(vertex_count, edges);
```

## 時間計算量

$N$ を `vertex_count` とする。

- 関数全体: $O(N)$
- 戻り値の各メンバの参照: $O(1)$

使用メモリは、可変長版で $O(N)$、固定容量版で固定容量の作業領域と長さ $N-1$ の内部列を合わせて $O(\texttt{MAX_SIZE} + N)$ である。再帰は使用しない。

## 注意点

- `0 < vertex_count` が必要で、固定容量版では `vertex_count <= MAX_SIZE` も必要である。
- `edges` はちょうど `vertex_count - 1` 本で、頂点番号は `[0, vertex_count)`、グラフは連結かつ閉路なしでなければならない。
- 自己ループ、多重辺、閉路、非連結、辺数の不一致、範囲外の頂点、容量超過では `std::runtime_error` を送出する。
- `vertex_count == 1` では `product == 0`、3 つの辺のメンバは `-1`、両方の直径は $0$ となる。
- 長さ $0$ の 1 頂点のパスも許す。積は `long long` で返す。
