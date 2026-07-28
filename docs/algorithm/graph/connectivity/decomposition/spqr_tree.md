---
title: SPQR Tree (SPQR木)
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/spqr_tree.hpp
---

二重頂点連結な無向多重グラフを、直列・並列・三重頂点連結・1辺の skeleton に分解する。各 tree edge は両端の skeleton に1本ずつ現れる virtual edge の組に対応する。

## API

```cpp
enum class SPQRNodeType {
    S,
    P,
    Q,
    R
};
```

- `S`: 単純閉路を表す series node
- `P`: 同じ2頂点を結ぶ多重辺を表す parallel node
- `Q`: 入力辺1本を表す edge node
- `R`: 単純かつ三重頂点連結な rigid node

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

`is_virtual()` が `false` の辺はQ nodeだけにあり、`original_edge_id` が入力順の辺IDを表す。このとき `twin_node = twin_edge = -1` である。

`is_virtual()` が `true` の辺では `original_edge_id = -1` であり、`nodes[twin_node].skeleton_edges[twin_edge]` が反対側の virtual edge である。twin同士の端点は、向きを無視して一致する。

```cpp
struct SPQRTreeNode {
    SPQRNodeType type;
    vector<SPQRSkeletonEdge> skeleton_edges;
};
```

Q nodeだけで完結する1辺グラフを除き、Q nodeの skeleton は入力辺とvirtual edgeの2本からなる。他の種類の node では、Q nodeを明示するため全 skeleton edge がvirtual edgeになる。

```cpp
struct SPQRTreeResult {
    int vertex_count;
    vector<SPQRTreeNode> nodes;
    vector<pair<int, int>> tree_edges;
    vector<int> original_edge_q_node;
};
```

- `nodes`: SPQR木のnodeと各skeleton
- `tree_edges`: SPQR木の辺。各要素は `nodes` の添字対
- `original_edge_q_node[e]`: 入力辺 `e` を実辺として持つQ node

隣接するS node同士およびP node同士は併合されている。したがって、返るSPQR木はこの併合に関してreducedである。

```cpp
SPQRTreeResult spqr_tree(
    int vertex_count,
    const vector<pair<int, int>>& edges
);
```

頂点集合を $[0,N)$ とする無向多重グラフのSPQR木を返す。

## 時間計算量

頂点数を $N$、辺数を $M$ とする。各部分グラフについて、1頂点を除いたグラフの関節点を全頂点分調べて分離対を求める。本実装の厳密な最悪計算量上界は $O(MN(N+M))$ である。

入力の二重頂点連結性検査、skeletonのreduction、Q nodeの構築は合計 $O(N+M)$ である。

## 空間計算量

$O(N+M)$。

## 注意点

- 自己ループは扱わない。
- $N=2$ の多重辺および1辺だけの退化ケースは扱える。
- $N\geq 3$ では入力が連結かつ関節点を持たない必要がある。
- 端点範囲違反、自己ループ、空グラフ、二重頂点連結でない入力に対して `invalid_argument` を送出する。
- 頂点番号はすべて入力グラフの番号であり、skeletonごとの局所番号へ付け替えない。
