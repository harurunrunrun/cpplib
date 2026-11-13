---
title: Spectral Bisection (スペクトル二分割)
documentation_of: ../../../src/approximate/graph/spectral_bisection.hpp
---

重み付きラプラシアンの Fiedler ベクトルを反復法で近似し、成分の中央値で頂点数が均衡した 2 分割を作る。密行列は構築しない。固有値の重複や反復回数によって結果が変わり得るヒューリスティックであり、最小カットは保証しない。

## `spectral_bisection`

~~~cpp
BipartitionResult spectral_bisection(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    size_t maximum_iterations = 300,
    long double tolerance = 1e-12L
)
~~~

最大 $I = \texttt{maximum_iterations}$ 回、定数ベクトルと直交する部分空間上でシフトしたラプラシアンのべき乗法を行う。成分の変化の最大値が `tolerance` 以下になったら反復を終了する。値が等しい成分は頂点番号順に並べるため決定的である。

- 時間計算量: $O(E \log E + I(N+E)+N\log N)$
- 追加空間計算量: $O(N+E)$

## 注意点

- 頂点数が 2 未満、辺がない（総重み 0）、または `maximum_iterations == 0` の場合は番号順の均衡分割（`balanced_bipartition`）を返す
- `tolerance` は有限な非負値でなければならず、満たさなければ `std::invalid_argument` を送出する
- 自己ループ・平行辺・重みの扱いは Weighted Undirected Graph Input に従う
