---
title: Girvan-Newman Community Detection
documentation_of: ../src/approximate/graph/girvan_newman.hpp
---

辺媒介中心性が最大の辺を1本ずつ除去し、連結成分をcommunityとするGirvan--Newman法である。このAPIは**重みなし無向グラフ**を対象とする。

## `GirvanNewmanResult`

- `labels[v]`: 頂点`v`のcommunity番号。頂点番号を左から走査した順に`0,1,...`へ正規化される
- `community_count`: 最終的な連結成分数
- `removed_edges`: 実際に除去した辺。各辺の端点は昇順である
- `rounds`: 実行した除去round数。`removed_edges.size()`と等しい

各fieldの参照は時間`O(1)`、`labels`と`removed_edges`の走査はそれぞれ時間`O(N)`, `O(R)`である。

## `girvan_newman`

~~~cpp
GirvanNewmanResult girvan_newman(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges,
    size_t target_community_count,
    size_t maximum_rounds = numeric_limits<size_t>::max()
)
~~~

Brandes法で現在残っている全辺の重みなし辺媒介中心性を再計算し、最大の辺を1本除去する。中心性が等しい場合は端点pairが辞書順最小の辺を選ぶ。これを連結成分数が`target_community_count`以上になるか、辺がなくなるか、`maximum_rounds`回に達するまで繰り返す。

`R`を実際の除去round数とすると、

- 時間計算量: `O((R + 1) N (N + E))`
- 追加空間計算量: `O(N + E)`

各roundのBrandes法が`O(N(N+E))`、連結成分計算が`O(N+E)`である。辺は1本ずつ除去するので`R <= min(E, maximum_rounds)`である。

### 入力規則と保証

- self-loopは無視する
- 同じ無向辺の重複は1本へまとめる
- 頂点番号が範囲外なら`std::out_of_range`を送出する
- 空グラフでは`target_community_count == 0`だけを許す
- 非空グラフでは`1 <= target_community_count <= N`を要求する
- 入力が初めから目標数より多くの連結成分を持つ場合、辺の除去だけでは目標へ到達できないため`std::invalid_argument`を送出する
- 目標未達の結果を返すのは`maximum_rounds`へ到達した場合だけである。この場合は`community_count < target_community_count`になり得る

辺を除去する階層的heuristicであり、modularity最大化や正解communityの復元は保証しない。
