---
title: Greedy Dominating Set (貪欲支配集合)
documentation_of: ../../../src/approximate/graph/dominating_set.hpp
---

~~~cpp
vector<size_t> approximate::graph::greedy_dominating_set(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges
)
~~~

まだ支配されていない頂点を閉近傍に最も多く含む頂点を、繰り返し選ぶ。同数なら頂点番号が小さい方を選ぶ。最大次数を $\Delta$ とすると、得られる集合の大きさは最小支配集合の $H_{\Delta+1}$ 倍以下である（$H_k$ は調和数）。

頂点数を $N$、辺数を $E$ とすると、

- 時間計算量: $O((N+E)\log N)$
- 追加空間計算量: $O(N+E)$

## 注意点

- 平行辺は 1 本として扱う。自己ループは、その頂点が閉近傍に既に含まれるため結果に影響しない。
- 範囲外の端点には `std::out_of_range` を送出する。
