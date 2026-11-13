---
title: Greedy Matching (貪欲マッチング)
documentation_of: ../../../src/approximate/graph/matching.hpp
---

無向グラフの貪欲マッチングである（`approximate::graph` 名前空間）。自己ループは選ばない。

## `MatchingResult`

- `edge_indices`: 選んだ辺の入力での添字（選択順）
- `mate[v]`: 頂点 `v` の相手。マッチしていなければ `vertex_count`

## `greedy_matching`

~~~cpp
MatchingResult greedy_matching(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges
)
~~~

入力順に、両端がともに未使用の辺を選ぶ。極大マッチングを返し、その辺数は最大マッチングの半分以上である。

- 時間計算量: $O(N+E)$
- 追加空間計算量: $O(N+E)$（返り値を含む）

## `greedy_weighted_matching`

~~~cpp
template<class Weight>
MatchingResult greedy_weighted_matching(
    size_t vertex_count,
    const vector<tuple<size_t, size_t, Weight>>& edges
)
~~~

正の重みの辺を重みの降順に処理する。重みが等しい場合は、正規化した端点対、入力での辺の添字の順に優先する。非負重みの最大重みマッチングに対する $1/2$ 近似になる。重みが 0 以下の辺は選ばない。

- 時間計算量: $O(N+E\log E)$
- 追加空間計算量: $O(N+E)$

## 注意点

- 範囲外の端点には `std::out_of_range` を送出する
- 浮動小数点の重みは有限でなければならず、非有限値には `std::invalid_argument` を送出する
