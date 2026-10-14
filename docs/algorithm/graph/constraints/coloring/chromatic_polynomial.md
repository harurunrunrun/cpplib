---
title: Chromatic Polynomial (彩色多項式) [chromatic_polynomial]
documentation_of: ../../../../../src/algorithm/graph/constraints/coloring/chromatic_polynomial.hpp
---

`chromatic_polynomial<T>(N, edges)` は、$k$ 色での適正彩色数を与える
多項式 $P(k)$ の係数を定数項から返す。
独立集合の集合冪級数から、順序付き集合分割の個数を power projection で求め、
下降階乗基底を単項式基底へ変換する。多重辺を許し、自己ループがあれば零多項式を返す。

## 時間計算量

$O(N^2 2^N+N^3)$、追加空間 $O(N2^N)$。

## 注意点

- $0\le N\le24$。指数時間・空間を使うため、実用上の上限は実行環境に依存する。
- `T` は標数が $N$ より大きい体とする。階乗の逆元を使う。
- 空グラフの返り値は長さ $N+1$。頂点数ゼロでは `{1}`。
