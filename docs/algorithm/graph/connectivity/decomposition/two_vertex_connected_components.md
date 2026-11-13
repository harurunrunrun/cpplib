---
title: Two Vertex Connected Components (二重頂点連結成分分解) [biconnected_components]
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/two_vertex_connected_components.hpp
---

無向グラフの二重頂点連結成分分解。

## 関数

```cpp
TwoVertexConnectedComponentsResult two_vertex_connected_components(
    int n,
    const vector<pair<int, int>>& edges
)
```

`TwoVertexConnectedComponentsResult` は次のメンバを持つ。

```cpp
int count;
vector<int> articulation;
vector<vector<int>> vertex_groups;
vector<vector<int>> edge_groups;
```

- `count`: 成分の個数。
- `articulation`: 関節点を昇順に並べたもの。
- `vertex_groups[c]`: 成分 `c` の頂点を昇順に並べたもの。
- `edge_groups[c]`: 成分 `c` に属する入力辺の ID を昇順に並べたもの。

孤立点は、頂点 1 個・辺 0 本の成分として返す。

## 時間計算量

頂点数を $N$、辺数を $M$、関節点数を $A$ とする。

- `two_vertex_connected_components`: $O(N + M \log (M + 1) + A \log (A + 1))$

Lowlink（関節点を求めるため `lowlink` を別に呼ぶ）と成分を求める DFS は $O(N + M)$。各成分の頂点と辺 ID をソートする時間の合計が $O(M \log (M + 1))$、関節点のソートが $O(A \log (A + 1))$。孤立点を含む戻り値の総要素数は $O(N + M)$。

## 空間計算量

- 戻り値を含めて $O(N + M)$

## 注意点

- `n` は非負、辺の端点は $[0, n)$ でなければならない。違反時は `std::runtime_error` を送出する。記載した計算量には引数の検査と戻り値の構築を含む。
- 多重辺を扱える。自己ループは無視する。自己ループの辺 ID はどの `edge_groups` にも含まれず、自己ループ以外の辺を持たない頂点は孤立点と同じく頂点 1 個・辺 0 本の成分になる。
- DFS は再帰で実装されており、再帰の深さは最大 $N$ になる。
