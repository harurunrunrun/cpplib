---
title: Markov Clustering (マルコフクラスタリング)
documentation_of: ../../../src/approximate/graph/markov_clustering.hpp
---

重み付き無向グラフのランダムウォーク行列に対して拡張（expansion）・膨張（inflation）・枝刈り（pruning）を繰り返す Markov Clustering Algorithm（MCL）である。密行列を使うため、小規模から中規模のグラフを対象とする。

## `MarkovClusteringResult`

- `labels[v]`: 頂点 `v` のコミュニティ番号。頂点順に `0, 1, ...` へ正規化される
- `community_count`: コミュニティ数
- `transition_matrix[row][column]`: 最終的な列確率行列
- `modularity`: 返すコミュニティの、入力グラフに対するモジュラリティ
- `iterations`: 実行した反復回数
- `converged`: 行列要素の変化の最大値が `tolerance` 以下になったか

各フィールドの参照は時間 $O(1)$ である。`labels` の走査は時間 $O(N)$、`transition_matrix` の走査は時間 $O(N^2)$ である。

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

次を繰り返す。

1. 列確率行列を `expansion_power` 乗してランダムウォークを展開する
2. 各要素を `inflation` 乗して密な領域を強調する
3. 正規化前の膨張後の値が `pruning_threshold` 未満の要素を 0 にする。ただし各列の最大要素は必ず残す
4. 枝刈り後の各列を正規化する

したがって `pruning_threshold` は、列の正規化後の確率ではなく、`inflation` 乗した直後の正規化前の値に対する閾値である。

最終行列の各列で最大値を取る頂点をアトラクタとし、各頂点からそのアトラクタへの辺を無向とみなしたときの連結成分をコミュニティとして返す。最大値が同じ場合は頂点番号が最小のアトラクタを選ぶ。

反復回数を $I$、$P = \texttt{expansion_power}$ とすると、

- 時間計算量: $O(E \log E + N^2 + I ((P - 1) N^3 + N^2))$
- 追加空間計算量: $O(N^2 + E)$

## 注意点

- 辺重みは有限な非負値でなければならない
- 重み 0 の辺と自己ループの入力は無視する。同じ無向辺の重みは加算する
- `expansion_power >= 2`、`inflation` は有限かつ 1 より大きい、`self_loop_weight`, `pruning_threshold`, `tolerance` は有限かつ非負でなければならない。満たさなければ `std::invalid_argument` を送出する
- 列の重み和が 0 なら、その頂点だけに確率 1 の自己ループを設定する
- `maximum_iterations == 0` では初期の列確率行列からコミュニティを抽出し、`converged == false` を返す
- 入力や途中の計算が表現範囲を超える場合は `std::overflow_error`、密行列の要素数が `size_t` で表せない場合は `std::length_error` を送出する
- 各列は有限かつ非負で、和は丸め誤差の範囲で 1 になる。同じ入力とパラメータからは決定的に同じ結果を返す
- MCL はヒューリスティックであり、`maximum_iterations` 以内の収束、モジュラリティの最大化、正解コミュニティの復元は保証しない
