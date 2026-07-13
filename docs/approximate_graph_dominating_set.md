---
title: Greedy Dominating Set (貪欲支配集合)
documentation_of: ../src/approximate/graph/dominating_set.hpp
---

~~~cpp
vector<size_t> approximate::graph::greedy_dominating_set(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges
)
~~~

未支配頂点を閉近傍に最も多く含む頂点を反復して選ぶ。同数なら頂点番号が小さい方を選ぶ。最大次数を $\Delta$ とすると、最小支配集合の $H_{\Delta+1}$ 倍以下の頂点数になる。

- 時間計算量: $O((N+E)\log N)$
- 追加空間計算量: $O(N+E)$

平行辺は1辺として扱い、自己ループは閉近傍に既に含まれるため結果へ影響しない。範囲外の端点には `std::out_of_range` を送出する。
