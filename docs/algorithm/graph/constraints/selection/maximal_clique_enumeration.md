---
title: Maximal Clique Enumeration (極大クリーク列挙)
documentation_of: ../../../../../src/algorithm/graph/constraints/selection/maximal_clique_enumeration.hpp
---

無向グラフの極大クリークを重複なくすべて列挙する。

## API

```cpp
vector<vector<int>> enumerate_maximal_cliques(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

各極大クリークを昇順の頂点列で表し、クリークの列全体を辞書順で返す。多重辺は1本へまとめ、自己ループは無視する。

## 時間計算量

頂点数を $N$、出力されるクリークの頂点数の総和を $L$、$W=64$ とする。

- 隣接ビット集合の構築: $O(N\lceil N/W\rceil+M)$
- pivot付きBron--Kerbosch法: 最悪 $O(3^{N/3}\lceil N/W\rceil)$
- 出力内の整列: $O(L\log N+C\log(C+1))$。$C$ は極大クリーク数

## 空間計算量

出力を除いて $O(N\lceil N/W\rceil+N^2/W)$、出力に $O(L)$。

## 注意点

辺の端点は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。空グラフでは、空集合を唯一の極大クリークとして返す。極大クリークは「これ以上頂点を追加できない」クリークであり、頂点数最大のクリークだけを意味しない。
