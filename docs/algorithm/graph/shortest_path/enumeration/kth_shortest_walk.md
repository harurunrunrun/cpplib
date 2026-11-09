---
title: K-th Shortest Walk (K番目最短歩) [k_shortest_walk]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/enumeration/kth_shortest_walk.hpp
---

非負重みの有向グラフで、頂点と辺の再訪を許す歩道（walk）の距離を小さい順に求める。

## 辺

```cpp
template <class T>
struct KthShortestWalkEdge { int to; T cost; };
```

`to` は行き先、`cost` は非負の辺の重みである。

## 列挙

```cpp
template <class T>
std::vector<T> kth_shortest_walks(
    const std::vector<std::vector<KthShortestWalkEdge<T>>>& graph,
    int source, int target, int k
)
```

`graph[v]` は `v` から出る `KthShortestWalkEdge<T>` の列である。

戻り値は `source` から `target` への歩道の距離を小さい順に最大 `k` 個並べた列である。歩道が `k` 個未満しか存在しない場合は存在する分だけ返す。`source == target` のときは空の歩道の距離 $0$ も含む。`k == 0` なら空列を返す。

## 指定順位の値

```cpp
template <class T>
std::optional<T> kth_shortest_walk(graph, source, target, int k)
template <class T>
std::optional<T> kth_shortest_walk_1indexed(graph, source, target, int k)
```

- `kth_shortest_walk` の `k` は 0-indexed で、`k == 0` が最短の歩道を表す。
- `kth_shortest_walk_1indexed` の `k` は 1-indexed で、`k == 1` が最短の歩道を表す。`k <= 0` は範囲外である。

指定順位まで歩道が存在しない場合は `std::nullopt` を返す。距離列全体は保持せず、指定順位が確定した時点で終了する。

## 歩道の数え方

通る辺の列が異なる歩道は別のものとして数える。多重辺のどれを選ぶかが異なる場合も別の歩道であり、距離が同じでもそれぞれが順位をもつ。

重み $0$ の閉路も許す。このとき同じ距離の歩道が複数個、あるいはいくらでも多く現れることがある。`source == target` では空の歩道が最初の候補になる。

## アルゴリズム

逆グラフ上の Dijkstra 法で各頂点から `target` までの最短距離と最短路木を求める。木に選ばれなかった各辺 $(u, v)$ を sidetrack とし、

$$
\mathrm{cost}(u, v) + \mathrm{dist}[v] - \mathrm{dist}[u]
$$

を追加コストとして永続 leftist heap に格納する。Eppstein の方法に従い sidetrack 列を優先度付きキューで展開するため、順位ごとに全辺や全頂点を展開し直すことはない。重み $0$ の閉路、多重辺、最短距離が等しい辺も、それぞれ独立した sidetrack として列挙する。

## 計算量

$N$ を頂点数、$M$ を辺数、$K$ を実際に求める順位の個数とする。

- `kth_shortest_walks(..., k)`: $K = k$ として、時間 $O((N + M)\log(N + M + 1) + K\log(K + 1))$
- `kth_shortest_walk(..., k)`: $K = k + 1$ として、時間 $O((N + M)\log(N + M + 1) + K\log(K + 1))$
- `kth_shortest_walk_1indexed(..., k)`: $K = k$ として、時間 $O((N + M)\log(N + M + 1) + K\log(K + 1))$

`kth_shortest_walks(..., 0)` は入力の検査だけを行い、時間 $O(N + M)$ である。

$S$ を `target` へ到達可能な部分にある sidetrack の個数とすると、逆辺、距離、最短路木、永続ヒープ、列挙用のキューを合わせた作業領域は

$$
O(N + M + S\log(S + 1) + K)
$$

である。頂点ごとに順位の回数だけ展開する素朴な方法で必要になる $O(KM)$ の候補領域は使わない。`kth_shortest_walks` はこれに加えて戻り値の $O(K)$ を使い、指定順位版は距離列を保存しない。

## 注意点

- 列挙版と 0-indexed 版は `k >= 0`、1-indexed 版は `k > 0` でなければならない。順位、端点、辺の行き先が範囲外の場合や、負の重みの辺がある場合は `runtime_error` を送出する。歩道が足りない場合は、短い列または `std::nullopt` で表す。
- `T` は `T()` が重み $0$ を表し、コピー、比較、加算、減算ができる型とする。歩道の距離、Dijkstra 法での加算、sidetrack の追加コスト、列挙中の加算の結果は `T` で表現できなければならない。
