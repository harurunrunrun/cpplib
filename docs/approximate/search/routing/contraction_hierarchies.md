---
title: Contraction Hierarchies (縮約階層)
documentation_of: ../../../../src/approximate/search/routing/contraction_hierarchies.hpp
---

頂点を順番に縮約し、縮約頂点を通る最短路が失われる場合だけ shortcut を追加する。有界 witness search が同コスト以下の迂回路を見つけた候補は追加しない。問い合わせは shortcut 込みグラフ上の上向き双方向 Dijkstra で、厳密距離を返す。

`graph_type` は `std::vector<std::vector<std::pair<int, Cost>>>`、
`cost_type` は `Cost` の別名である。

## 公開 API

```cpp
explicit ContractionHierarchy(
    graph_type graph,
    std::vector<int> contraction_order = {}
);
```

`contraction_order` は先に縮約する頂点から並べた全頂点の置換。空なら出次数の小さい順を安定ソートして使う。

```cpp
int vertex_count() const noexcept;
int rank(int vertex) const;
std::size_t shortcut_count() const noexcept;
std::optional<Cost> query_distance(int source, int target) const;
```

- `rank`: 小さいほど早く縮約された頂点。
- `shortcut_count`: witness がなく実際に追加した shortcut 数。
- `query_distance`: 順方向は上向き弧、逆方向は元の下向き弧の逆を探索する。到達不能なら `std::nullopt`。

## 時間計算量

最終 shortcut 込み弧数を $M$、検査した shortcut 候補数を $P$ とする。

- 構築: 最悪時間 $O(P(M+V)\log V)$、空間 $O(M+V)$
- `vertex_count`, `rank`, `shortcut_count`: $O(1)$
- 問い合わせ: 訪問する上向き頂点数・弧数を $V_u,M_u$ として
  $O((M_u+V_u)\log V_u)$、追加空間 $O(V)$

## 注意点

- 縮約順は正しさに影響しないが、shortcut 数と問い合わせ速度を大きく左右する。
- witness search は現在未縮約の部分グラフで、候補 shortcut コストを上限に実行する。
- この API は距離オラクルであり、shortcut 展開による頂点列復元は提供しない。
- 負辺、順序の重複・欠落・範囲外頂点は `std::invalid_argument`。
- 密グラフや悪い順序では shortcut と前処理時間が大きくなる。

## English

`ContractionHierarchy` contracts vertices, runs a cost-bounded witness search for every necessary predecessor/successor pair, and inserts a shortcut only when no equal-or-better witness exists. Queries are exact directed upward bidirectional Dijkstra searches. Preprocessing is output/order sensitive; with $P$ candidates and $M$ augmented arcs its conservative worst-case bound is $O(P(M+V)\log V)$.
