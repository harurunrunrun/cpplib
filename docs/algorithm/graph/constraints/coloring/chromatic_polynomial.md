---
title: Chromatic Polynomial (彩色多項式) [chromatic_polynomial]
documentation_of: ../../../../../src/algorithm/graph/constraints/coloring/chromatic_polynomial.hpp
---

`chromatic_polynomial<T>(N, edges)` は、$k$ 色での適正彩色の個数を与える多項式 $P(k)$ の係数を、定数項から順に返す。
独立集合の集合冪級数から、べき射影（power projection）によって独立集合への順序付き分割の個数を求め、下降階乗基底から単項式基底へ変換する。多重辺を許し、自己ループがあれば零多項式を返す。

## 時間計算量

$O(N^2 2^N + N^3)$、追加空間 $O(N 2^N)$。

## 注意点

- $0 \le N \le 24$ でなければならず、範囲外なら `std::invalid_argument` を送出する。指数時間・指数空間を使うため、実用上の上限は実行環境に依存する。
- 辺の端点が $[0, N)$ の範囲外なら `std::out_of_range` を送出する。
- `T` は標数が $N$ より大きい体とする。階乗の逆元を使う。
- 戻り値の長さは常に $N + 1$ である（頂点数 0 なら `{1}`）。
