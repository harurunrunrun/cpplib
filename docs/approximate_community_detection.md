---
title: Weighted Community Detection Common API
documentation_of: ../src/approximate/graph/community_detection.hpp
---

`approximate::graph` 名前空間のcommunity detectionで共通に使う結果型とmodularity計算である。自己辺を無視し、平行辺の重みを加算する。

# CommunityResult

`label[v]` は頂点 `v` のcommunity番号である。番号は頂点番号順の初出が0, 1, ...となるよう正規化される。`community_count`、重み付きundirected modularity `modularity`、アルゴリズムが実行した反復数 `iterations` を返す。

# canonical_community_labels

~~~cpp
vector<size_t> canonical_community_labels(const vector<size_t>& labels)
~~~

同値関係を変えずにcommunity番号を連続化する。

- 時間計算量: $O(N\log N)$
- 追加空間計算量: $O(N)$

# community_modularity

~~~cpp
long double community_modularity(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    const vector<size_t>& labels
)
~~~

$Q=\sum_C(w_C/m-(d_C/(2m))^2)$ を返す。$w_C$ はcommunity内部辺の重み和、$d_C$ は次数和、$m$ は全辺重み和である。辺がない場合は0を返す。

- 時間計算量: $O(E\log E+N\log N)$
- 追加空間計算量: $O(N+E)$

label数が頂点数と異なる場合は `std::invalid_argument` を送出する。辺の入力条件は Weighted Undirected Graph Input に従う。
