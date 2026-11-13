---
title: st-numbering (st番号付け) [st_numbering]
documentation_of: ../../../../../src/algorithm/graph/connectivity/decomposition/st_numbering.hpp
---

無向多重グラフの頂点を、指定した 2 頂点を両端とする双極向き付け（bipolar orientation）になる順に並べる。DFS の Lowlink から Even--Tarjan の挿入順を構成する。再帰を使わないため、深いグラフにも使える。

## 結果

```cpp
struct STNumberingResult {
    vector<int> number;
    vector<int> order;
};
```

- `number[v]` は頂点 `v` の番号であり、`number` は $0$ 以上 $N$ 未満の整数の置換である。
- `order[i]` は番号 `i` の頂点であり、`order` は `number` の逆置換である。
- `order.front() == source`、`order.back() == sink` が成り立つ。
- `source`, `sink` 以外の各頂点は、番号が自分より小さい隣接頂点と大きい隣接頂点を少なくとも 1 つずつ持つ。

## st-numbering

```cpp
optional<STNumberingResult> st_numbering(
    int vertex_count,
    const vector<pair<int, int>>& edges,
    int source,
    int sink
)
```

条件を満たす番号付けがあればそれを返し、なければ `std::nullopt` を返す。多重辺は許すが、自己ループは許さない。

- 頂点数 1 で `source == sink == 0` の場合は `{0}` を返す（このとき辺は検査しない）。
- 頂点数 2 以上で `source == sink` の場合は、辺を検査せずに `std::nullopt` を返す。

## 時間計算量

頂点数を $N$、辺数を $M$ とする。

- $O(N + M)$

## 空間計算量

- $O(N + M)$

## 注意点

`vertex_count` が負、`source` または `sink` が範囲外（頂点数 0 の場合を含む）、辺の端点が範囲外、または自己ループを含む場合は `std::runtime_error` を送出する。これらの検査は、頂点数 1 の場合や `source == sink` の場合を含め、常に結果を返す前に行う。
