---
title: Markov Clustering (マルコフクラスタリング)
documentation_of: ../src/approximate/graph/markov_clustering.hpp
---

重み付き無向グラフ上のrandom walk行列にexpansion・inflation・pruningを反復するMarkov Clustering Algorithm (MCL) である。密行列を使うため、小規模から中規模のグラフを対象とする。

## `MarkovClusteringResult`

- `labels[v]`: 頂点`v`のcommunity番号。頂点順に`0,1,...`へ正規化される
- `community_count`: community数
- `transition_matrix[row][column]`: 最終的な列確率行列
- `modularity`: 入力グラフに対する返却communityのmodularity
- `iterations`: 実行した反復回数
- `converged`: 行列の最大要素変化が`tolerance`以下になったか

field参照は時間`O(1)`である。`labels`の走査は時間`O(N)`、`transition_matrix`の走査は時間`O(N^2)`である。

## `markov_clustering`

~~~cpp
MarkovClusteringResult markov_clustering(
    size_t vertex_count,
    const vector<WeightedUndirectedEdge>& edges,
    size_t expansion_power = 2,
    long double inflation = 2,
    long double self_loop_weight = 1,
    long double pruning_threshold = 1e-12,
    size_t maximum_iterations = 100,
    long double tolerance = 1e-10
)
~~~

次を反復する。

1. 列確率行列を`expansion_power`乗してrandom walkを展開する
2. 各要素を`inflation`乗して密な領域を強調する
3. 未正規化のinflated値が`pruning_threshold`未満の要素を0にする。ただし各列の最大要素は必ず残す
4. pruning後の各列を正規化する

したがって`pruning_threshold`は、列正規化後の確率ではなく、`inflation`乗直後の未正規化値に対する閾値である。

最終行列の各列で最大値を取る頂点をattractorとし、頂点からattractorへの辺を無向に見た連結成分をcommunityとして返す。同値の最大値では頂点番号最小のattractorを選ぶ。

反復回数を`I`、`P = expansion_power`とすると、

- 時間計算量: `O(E log E + N^2 + I ((P - 1) N^3 + N^2))`
- 追加空間計算量: `O(N^2 + E)`

### 入力規則

- 辺重みは有限な非負値でなければならない
- zero-weight辺とself-loop入力は無視する。同じ無向辺の重みは加算する
- `expansion_power >= 2`
- `inflation`は有限で`> 1`
- `self_loop_weight`, `pruning_threshold`, `tolerance`は有限で非負
- 列の重み和が0なら、その頂点だけに確率1のself-loopを設定する
- `maximum_iterations == 0`では初期列確率行列からcommunityを抽出し、`converged == false`を返す

入力や途中計算が表現範囲を超える場合は`std::overflow_error`、密行列の要素数が`size_t`で表せない場合は`std::length_error`を送出する。

### 保証と注意

各列は有限・非負で、和は丸め誤差の範囲で1になる。同じ入力とparameterからは決定的に同じ結果を返す。一方、MCLはheuristicであり、`maximum_iterations`以内の収束、modularity最大化、正解communityの復元を保証しない。
