---
title: st-numbering (st番号付け)
documentation_of: ../src/algorithm/graph/st_numbering.hpp
---

無向多重グラフの頂点を、指定した2頂点を両端とするbipolar orientationになる順に並べる。DFS low-linkからEven--Tarjanの挿入順を構成する。再帰を使わないため、深いグラフにも利用できる。

# 結果

```cpp
struct STNumberingResult {
    vector<int> number;
    vector<int> order;
};
```

- `number[v]` は頂点 `v` の番号であり、`0` 以上 `N` 未満の置換である。
- `order[i]` は番号 `i` の頂点であり、`number` の逆置換である。
- `order.front() == source`、`order.back() == sink`。
- `source`, `sink` 以外の各頂点は、番号が小さい隣接頂点と大きい隣接頂点を少なくとも1つずつ持つ。

# st-numbering

```cpp
optional<STNumberingResult> st_numbering(
    int vertex_count,
    const vector<pair<int, int>>& edges,
    int source,
    int sink
)
```

条件を満たす番号付けがあれば返し、なければ `nullopt` を返す。多重辺を許すが自己loopは許さない。1頂点で `source == sink == 0` の場合は `{0}` を返す。2頂点以上で `source == sink` の場合は `nullopt`。

## 時間計算量

- `O(N+M)`

## 空間計算量

- `O(N+M)`

# 例外

`vertex_count`、端点、`source`、`sink` が範囲外、または自己loopを含む場合は `runtime_error` を送出する。
