---
title: Weighted Clique Polynomial (重み付きクリーク多項式) [enumerate_cliques]
documentation_of: ../../../../../src/algorithm/graph/constraints/selection/weighted_clique_polynomial.hpp
---

`weighted_clique_polynomial(weights, edges)` は長さ $N+1$ の係数列を返す。
第 $k$ 係数は、サイズ $k$ のクリーク $C$ に対する $\prod_{v\in C}w_v$ の総和。
空クリークの係数は $1$。全係数を足すと全クリークの重み和、定数項を除くと空でないクリークの重み和になる。

次数の小さい頂点から除去して辺を向き付け、各クリークをその最初の頂点から一度だけ列挙する。

## 時間計算量

縮退数を $d$ として $O((N+M)\log(N+1)+Nd^2\log(N+1)+N2^d)$。
追加空間は $O(N+M+d^2)$。再帰深さはクリークの大きさ以下。

## 注意点

- 無向グラフの各辺を一度与える。重複辺はまとめる。自己ループ・範囲外の端点は `invalid_argument`。
- $d\le63$。これを超える場合は `length_error`。クリーク数が多いグラフには指数時間がかかる。
- `T` は整数や剰余型など、加算・乗算と $0,1$ の構築をサポートする型。
