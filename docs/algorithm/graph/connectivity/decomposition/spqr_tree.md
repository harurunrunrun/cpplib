---
title: SPQR Tree (SPQR木)
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/spqr_tree.hpp
---

二重頂点連結な無向多重グラフを、直列・並列・三重頂点連結・辺 1 本のスケルトンに分解する。SPQR 木の各辺は、両端のノードのスケルトンに 1 本ずつ現れる仮想辺の組に対応する。

## API

```cpp
enum class SPQRNodeType {
    S,
    P,
    Q,
    R
};
```

- `S`: 単純閉路を表す直列（series）ノード
- `P`: 同じ 2 頂点を結ぶ多重辺を表す並列（parallel）ノード
- `Q`: 入力辺 1 本を表す辺ノード
- `R`: 単純かつ三重頂点連結な剛（rigid）ノード

```cpp
struct SPQRSkeletonEdge {
    int from;
    int to;
    int original_edge_id;
    int twin_node;
    int twin_edge;

    bool is_virtual() const noexcept;
};
```

`is_virtual()` は `original_edge_id == -1` のとき `true` を返す。

- `is_virtual()` が `false` の辺（実辺）は Q ノードにだけ現れ、`original_edge_id` が入力順の辺 ID を表す。このとき `twin_node` と `twin_edge` はともに $-1$ である。
- `is_virtual()` が `true` の辺（仮想辺）では `original_edge_id` が $-1$ であり、`nodes[twin_node].skeleton_edges[twin_edge]` が反対側の仮想辺である。対になる仮想辺どうしの端点は、向きを無視すれば一致する。

```cpp
struct SPQRTreeNode {
    SPQRNodeType type;
    vector<SPQRSkeletonEdge> skeleton_edges;
};
```

辺 1 本だけのグラフで Q ノード 1 個だけからなる場合を除き、Q ノードのスケルトンは入力辺 1 本と仮想辺 1 本からなる。他の種類のノードでは、Q ノードを明示的に持つため、スケルトンの辺はすべて仮想辺になる。

```cpp
struct SPQRTreeResult {
    int vertex_count;
    vector<SPQRTreeNode> nodes;
    vector<pair<int, int>> tree_edges;
    vector<int> original_edge_q_node;
};
```

- `nodes`: SPQR 木のノードと、それぞれのスケルトン
- `tree_edges`: SPQR 木の辺。各要素は `nodes` の添字の組
- `original_edge_q_node[e]`: 入力辺 `e` を実辺として持つ Q ノードの添字

隣接する S ノードどうし、および P ノードどうしは併合済みである。したがって、返される SPQR 木はこの併合に関して既約（reduced）である。

```cpp
SPQRTreeResult spqr_tree(
    int vertex_count,
    const vector<pair<int, int>>& edges
);
```

頂点集合を $[0, N)$ とする無向多重グラフの SPQR 木を返す。

## 時間計算量

頂点数を $N$、辺数を $M$ とする。

- `SPQRSkeletonEdge::is_virtual()`: $O(1)$
- `spqr_tree()`: $O(N + M)$

`spqr_tree()` の計算量には、入力の二重頂点連結性の検査、SPQR 分解、スケルトンの併合、Q ノードの構築をすべて含む。

## 空間計算量

$O(N + M)$。

## 注意点

- 自己ループは扱わない。
- $N = 2$ の多重辺、および辺 1 本だけの退化したケースは扱える。
- $N \ge 3$ では、入力が連結かつ関節点を持たない必要がある。
- 頂点数が 2 未満、辺集合が空、端点が範囲外、自己ループを含む、または二重頂点連結でない入力に対しては `std::invalid_argument` を送出する。
- 頂点数または辺数が実装上の上限を超える場合は `std::length_error` を送出する。
- スケルトン中の頂点番号はすべて入力グラフの番号であり、スケルトンごとの局所番号には付け替えない。
