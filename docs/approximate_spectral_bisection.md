---
title: Spectral Bisection (スペクトル二分割)
documentation_of: ../src/approximate/graph/spectral_bisection.hpp
---

重み付きLaplacianのFiedler vectorを反復法で近似し、成分の中央値で頂点数が平衡な2分割を作る。密行列を構築しない。固有値の重複や反復回数により結果が変わり得るheuristicであり、最小cutを保証しない。

## spectral_bisection

~~~cpp
BipartitionResult spectral_bisection(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    size_t maximum_iterations = 300,
    long double tolerance = 1e-12L
)
~~~

`I` 回を上限として、定数vectorと直交する部分空間上でshifted Laplacianのpower iterationを行う。`tolerance` 以下の最大成分変化で反復を終了する。同値成分は頂点番号順に並べるため決定的である。

- 時間計算量: $O(E \log E + I(N+E)+N\log N)$
- 追加空間計算量: $O(N+E)$

辺がない場合と `maximum_iterations == 0` では番号順の平衡分割を返す。`tolerance` は有限な非負値でなければならない。

## 注意点

頂点番号は指定した頂点数の範囲内でなければならない。自己loop・平行辺・重みの扱いは各APIの記載に従い、明記した保証を除いて最適解を返すとは限らない。
