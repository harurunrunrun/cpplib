---
title: Minimum Cycle Basis (最小閉路基底)
documentation_of: ../../../../../src/algorithm/graph/specialized/subgraph_analysis/minimum_cycle_basis.hpp
---

非負重みの無向多重グラフについて、辺集合を $\mathbb F_2$ 上のベクトルとみなした閉路空間の、総重み最小の基底を求める。

## API

```cpp
struct MinimumCycleBasisEdge {
    int from;
    int to;
    long long weight;
};
```

無向辺と非負重みを表す。多重辺と自己ループを扱える。

```cpp
MinimumCycleBasisResult minimum_cycle_basis(
    int vertex_count,
    const vector<MinimumCycleBasisEdge>& edges
)
```

```cpp
struct MinimumCycleBasisResult {
    long long weight;
    vector<vector<int>> cycles;
};
```

- `weight`: 基底に選んだ閉路の重みの総和
- `cycles`: 各基底ベクトルに含まれる入力辺IDの昇順列

`cycles.size()` は $M-N+C$。$C$ は孤立頂点を含む連結成分数である。

## 時間計算量

頂点数を $N$、辺数を $M$、$W=64$、Horton候補数を $H\le NM$ とする。

- 全始点のDijkstra法: $O(N(M+N)\log N)$
- 最短路の辺ビット集合とHorton候補の構築: $O(N^2\lceil M/W\rceil+HM)$
- 候補の整列: $O(H\log(H+1)\lceil M/W\rceil)$
- GF(2)基底への挿入: $O(HM\lceil M/W\rceil)$

## 空間計算量

候補を含めて $O(H\lceil M/W\rceil+NM/W+N+M)$。

## 注意点

重みは非負で、辺の端点は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。距離と候補重みは符号付き128 bit整数で計算し、基底の総重みが `long long` を超える場合は `overflow_error` を送出する。同重み最短路は辺数、親辺IDの順で決定的に選ぶ。
