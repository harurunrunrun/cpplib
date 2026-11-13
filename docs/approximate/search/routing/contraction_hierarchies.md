---
title: Contraction Hierarchies (縮約階層)
documentation_of: ../../../../src/approximate/search/routing/contraction_hierarchies.hpp
---

頂点を順に縮約し、縮約した頂点を通る最短路が失われる場合だけショートカット辺（shortcut）を追加する。範囲を限った witness search で同じコスト以下の迂回路が見つかった候補は追加しない。問い合わせはショートカットを含むグラフ上の上向き双方向 Dijkstra 法で行い、厳密な距離を返す。

`graph_type` は `std::vector<std::vector<std::pair<int, Cost>>>`、`cost_type` は `Cost` の別名である。

## 公開 API

```cpp
explicit ContractionHierarchy(
    graph_type graph,
    std::vector<int> contraction_order = {}
);
```

`contraction_order` は、先に縮約する頂点から順に並べた全頂点の順列である。空なら、出次数の小さい順に安定ソートした順を使う。

```cpp
int vertex_count() const noexcept;
int rank(int vertex) const;
std::size_t shortcut_count() const noexcept;
std::optional<Cost> query_distance(int source, int target) const;
```

- `rank`: 頂点の縮約順位。小さいほど早く縮約された。
- `shortcut_count`: witness が見つからず実際に追加したショートカットの数。
- `query_distance`: 順方向は上向きの弧を、逆方向は下向きの弧を逆にたどって探索する。到達不能なら `std::nullopt` を返す。

## 時間計算量

頂点数を $V$、最終的なショートカットを含む弧数を $M$、検査したショートカット候補の数を $P$ とする。

- 構築: 最悪時間計算量 $O(P(M+V)\log V)$、空間計算量 $O(M+V)$
- `vertex_count`, `rank`, `shortcut_count`: $O(1)$
- 問い合わせ: 訪問する上向きの頂点数・弧数を $V_u$, $M_u$ として、時間計算量 $O((M_u+V_u)\log V_u)$、追加空間計算量 $O(V)$

## 注意点

- 縮約順は正しさに影響しないが、ショートカットの数と問い合わせの速度を大きく左右する。
- witness search は、その時点で未縮約の部分グラフ上で、候補ショートカットのコストを上限として実行する。
- この API は距離オラクルであり、ショートカットを展開して頂点列を復元する機能は提供しない。
- 負の辺、順序の重複・欠落・範囲外の頂点に対しては `std::invalid_argument` を送出する。
- 密なグラフや悪い順序では、ショートカットの数と前処理時間が大きくなる。
