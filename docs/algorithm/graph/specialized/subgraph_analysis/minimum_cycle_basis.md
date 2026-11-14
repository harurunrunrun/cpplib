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
- `cycles`: 各基底ベクトルに含まれる入力辺の ID を昇順に並べた列

`cycles.size()` は $M-N+C$ である。ここで $C$ は孤立頂点も 1 つと数えた連結成分の個数である。

## 時間計算量

頂点数を $N$、辺数を $M$、ワード長を $W=64$、Horton 候補の個数を $H\le NM$ とする。

- 全始点からの Dijkstra 法: $O(N(M+N)\log(M+N+1))$
- 最短路の辺ビット集合と Horton 候補の構築: $O(N^2\lceil M/W\rceil+H(\lceil M/W\rceil+N))$（候補の重みは立っているビットだけを走査して求め、各候補の辺数は $2N-1$ 以下）
- 候補の整列: $O(H\log(H+1)\lceil M/W\rceil)$
- $\mathbb F_2$ 上の基底への挿入: $O(HM\lceil M/W\rceil)$

## 空間計算量

候補を含めて $O(H\lceil M/W\rceil+NM/W+N+M)$。

## 注意点

- 頂点数と重みは非負で、辺の端点は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。
- 距離と候補の重みは符号付き 128 bit 整数で計算し、基底の総重みが `long long` に収まらない場合は `overflow_error` を送出する。
- 重みが等しい最短路が複数あるときは、辺数が少ないもの、次に親辺の ID が小さいものを選ぶため、結果は決定的である。
