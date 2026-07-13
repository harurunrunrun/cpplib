---
title: Greedy Matching (貪欲マッチング)
documentation_of: ../src/approximate/graph/matching.hpp
---

`approximate::graph` 名前空間にある無向グラフの貪欲matching。自己ループは選ばない。

# greedy_matching

入力順に、両端が未使用の辺を選ぶ。極大matchingを返し、最大matchingの半分以上の辺数を持つ。

- 時間計算量: $O(N+E)$
- 追加空間計算量: $O(N+E)$（返却値を含む）

# greedy_weighted_matching

~~~cpp
template<class Weight>
MatchingResult greedy_weighted_matching(
    size_t vertex_count,
    const vector<tuple<size_t, size_t, Weight>>& edges
)
~~~

正重みの辺を重み降順に処理する。同重みでは正規化した端点対、入力辺添字の順に選ぶ。非負重みの最大重みmatchingに対する1/2近似になる。0以下の重みは選ばない。

- 時間計算量: $O(N+E\log E)$
- 追加空間計算量: $O(N+E)$

`MatchingResult::edge_indices` は選択した入力辺添字、`mate[v]` は相手、未matchingなら `vertex_count` である。浮動小数点重みは有限でなければならない。範囲外の端点・非有限重みには例外を送出する。
