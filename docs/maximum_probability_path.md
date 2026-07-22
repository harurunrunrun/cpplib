---
title: Maximum Probability Path (最大確率路) [CHICAGO]
documentation_of: ../src/algorithm/graph/shortest_path/maximum_probability_path.hpp
---

各有向辺に $[0,1]$ の値を持つグラフで、始点から各頂点へ至る路上の辺値の積を最大化する。
辺値が1以下であるため、最大値を優先するDijkstra法として計算できる。

## `ProbabilityEdge<T>`

```cpp
template<class T>
struct ProbabilityEdge {
    int to;
    T probability;
};
```

`to` を終点、`probability` を辺値とする有向辺を表す。
無向辺は両方向の辺として格納する。

## `MaximumProbabilityPathResult<T>`

```cpp
template<class T>
struct MaximumProbabilityPathResult {
    vector<T> probability;
    vector<int> parent;
    vector<char> reachable;
};
```

- `probability[v]`: 始点から `v` までの積の最大値。到達不能なら $0$。
- `parent[v]`: 最大値を更新した直前の頂点。始点と到達不能頂点では `-1`。
- `reachable[v]`: 始点から `v` へ到達できるとき真。積が $0$ の到達可能路と到達不能を区別できる。

## `maximum_probability_path`

```cpp
template<class T>
MaximumProbabilityPathResult<T> maximum_probability_path(
    const vector<vector<ProbabilityEdge<T>>>& graph,
    int source
);
```

`source` から全頂点への最大積と復元用の親を返す。

## API別の時間計算量・空間計算量

$N=graph.size()$、$M$ を格納した有向辺数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_probability_path(graph, source)` | $O((N+M)\log(N+1))$ | $O(N+M)$ |

## 注意点

- `source` と各辺の `to` は $[0,N)$、`probability` は $[0,1]$ でなければならない。違反時は `runtime_error` を送出する。
- `T` は $0,1$、比較、乗算を扱える型とする。
- 浮動小数点型では積に丸め誤差が生じる。
- 同じ最大値を与える路が複数ある場合、`parent` の選択は辺の格納順などに依存する。
