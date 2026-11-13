---
title: Girvan–Newman Community Detection (Girvan–Newmanコミュニティ検出)
documentation_of: ../../../src/approximate/graph/girvan_newman.hpp
---

辺媒介中心性が最大の辺を 1 本ずつ除去し、残った連結成分をコミュニティとする Girvan–Newman 法である。この API は **重みなし無向グラフ** を対象とする。

## `GirvanNewmanResult`

- `labels[v]`: 頂点 `v` のコミュニティ番号。頂点番号の小さい順に走査して現れた順に `0, 1, ...` へ正規化される
- `community_count`: 最終的な連結成分数
- `removed_edges`: 実際に除去した辺。各辺の端点は昇順である
- `rounds`: 実行した除去ラウンド数。`removed_edges.size()` と等しい

各フィールドの参照は時間 $O(1)$、`labels` と `removed_edges` の走査はそれぞれ時間 $O(N)$, $O(R)$ である。

## `girvan_newman`

~~~cpp
GirvanNewmanResult girvan_newman(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges,
    size_t target_community_count,
    size_t maximum_rounds = numeric_limits<size_t>::max()
)
~~~

Brandes 法で、現在残っている全辺の重みなし辺媒介中心性を再計算し、最大の辺を 1 本除去する。中心性が等しい場合は端点の組が辞書順最小の辺を選ぶ。これを、連結成分数が `target_community_count` 以上になるか、辺がなくなるか、`maximum_rounds` 回に達するまで繰り返す。

$R$ を実際の除去ラウンド数とすると、

- 時間計算量: $O((R + 1) N (N + E))$
- 追加空間計算量: $O(N + E)$

各ラウンドの Brandes 法が $O(N(N+E))$、連結成分の計算が $O(N+E)$ である。辺は 1 本ずつ除去するので $R \le \min(E, \texttt{maximum_rounds})$ である。

## 注意点

- 自己ループは無視する
- 同じ無向辺の重複は 1 本にまとめる
- 頂点番号が範囲外なら `std::out_of_range` を送出する
- 空グラフでは `target_community_count == 0` だけを許す
- 非空グラフでは `1 <= target_community_count <= N` を要求し、満たさなければ `std::invalid_argument` を送出する
- 入力が初めから目標数より多くの連結成分を持つ場合、辺の除去だけでは目標に到達できないため `std::invalid_argument` を送出する
- 最短路数や辺媒介中心性の計算が `long double` で表現できない大きさになると `std::overflow_error` を送出する
- 目標未達の結果を返すのは `maximum_rounds` に到達した場合だけである。この場合は `community_count < target_community_count` になり得る

辺を除去する階層的なヒューリスティックであり、モジュラリティの最大化や正解コミュニティの復元は保証しない。
