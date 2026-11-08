---
title: Weighted Clique Polynomial (重み付きクリーク多項式) [enumerate_cliques]
documentation_of: ../../../../../src/algorithm/graph/constraints/selection/weighted_clique_polynomial.hpp
---

頂点重み付き無向グラフのクリーク多項式の係数を求める。

## API

```cpp
template <class T>
std::vector<T> weighted_clique_polynomial(
    const std::vector<T>& weights,
    const std::vector<std::pair<int, int>>& edges
)
```

頂点数を $N$ = `weights.size()` とし、長さ $N+1$ の係数列を返す。第 $k$ 係数は、大きさ $k$ のクリーク $C$ すべてについての $\prod_{v \in C} w_v$ の総和である。空クリークに対応する定数項は $1$ である。全係数の和は全クリークの重みの和、定数項を除いた和は空でないクリークの重みの和になる。

次数の小さい頂点から順に除去して辺を向き付け（縮退順序）、各クリークを順序が最初の頂点から一度だけ列挙する。

## 計算量

辺数を $M$、縮退数を $d$ とする。

- 時間計算量: $O((N + M) \log (N + 1) + N d^2 \log (N + 1) + N 2^d)$
- 追加の空間計算量: $O(N + M + d^2)$。再帰の深さはクリークの大きさ以下である。

## 注意点

- 無向グラフの各辺を 1 回ずつ与える。重複辺は 1 本にまとめる。自己ループや範囲外の端点があれば `invalid_argument` を送出する。
- $d \le 63$ でなければならない。これを超える場合は `length_error` を送出する。クリーク数が多いグラフでは指数時間がかかる。
- `T` は整数型や剰余型など、加算・乗算と `T(0)`, `T(1)` の構築をサポートする型とする。
