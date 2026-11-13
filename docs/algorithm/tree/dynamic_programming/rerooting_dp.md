---
title: Rerooting DP (全方位木 DP)
documentation_of: ../../../../src/algorithm/tree/dynamic_programming/rerooting_dp.hpp
---

全方位木 DP を行う。
隣接する部分木からの寄与をモノイドでまとめ、頂点を足す処理と辺を渡る処理をコールバックとして与える。

## 使い方

```cpp
template<auto Monoid, class AddVertex, class AddEdge>
struct RerootingDP;

RerootingDP<Monoid, AddVertex, AddEdge> dp(n, add_vertex, add_edge);
int id = dp.add_edge(u, v);
std::vector<S> result = dp.solve(root = 0);
```

- `Monoid` は非型テンプレート引数として渡すモノイドのオブジェクトで、その型は値の型 `S` を持ち、`Monoid.op(a, b)` と `Monoid.e()` を提供する。
- `add_vertex(S aggregate, int v) -> S` は、隣接する部分木からの寄与をまとめた値に頂点 `v` を足す。
- `add_edge(S value, int from, int to, int edge_id) -> S` は、`from` 側の値を辺 `edge_id` 越しに `to` への寄与に変換する。
- `dp.add_edge(u, v)` は無向辺を追加し、0-indexed の辺 ID を返す。
- `dp.solve(root)[v]` は `v` を根とした木全体の DP 値である。
- `dp.size()` は頂点数を返す。

## 時間計算量

- `RerootingDP(n, add_vertex, add_edge)`: $O(N)$
- `size`: $O(1)$
- `add_edge`: 償却 $O(1)$
- `solve`: $O(N)$ 回のモノイド演算、`add_vertex`、`add_edge` の呼び出しと、長さ $N$ の出力の構築

上記はモノイド演算と各コールバックを $O(1)$ とした計算量である。コールバックが重い場合は、その呼び出しコストを掛ける。

## 空間計算量

保存するグラフ、前後の累積、戻り値を含めて $O(N)$ である。

## 注意点

- 頂点と `root` は範囲内でなければならない。入力は無向の連結木とする。
- 端点の範囲外、閉路、辺数の不一致、非連結の場合は `std::runtime_error` を送出する。記載した計算量には木の検査を含む。
