---
title: Maximum Probability Path (最大確率路)
documentation_of: ../src/algorithm/graph/maximum_probability_path.hpp
---

各辺の成功確率が $[0,1]$ にあるグラフで、始点から各頂点までの経路上の確率の積を最大化する。
通常のDijkstra法で距離を小さい順に確定する代わりに、確率を大きい順に確定する。

# MaximumProbabilityPathEdge

```cpp
template<class T>
struct MaximumProbabilityPathEdge {
    int to;
    T probability;
};
```

`to` を終点、`probability` を辺の成功確率とする有向辺を表す。無向辺は両方向に追加する。

# MaximumProbabilityPathResult

```cpp
template<class T>
struct MaximumProbabilityPathResult {
    vector<T> probability;
    vector<int> parent;
    vector<char> reachable;
};
```

- `probability[v]`: 始点から `v` までの最大成功確率。到達不能なら $0$。
- `parent[v]`: 最大成功確率を更新した直前の頂点。始点と到達不能な頂点では `-1`。
- `reachable[v]`: 始点から `v` へ到達できるなら真。確率 $0$ の経路も到達可能として区別する。

# maximum_probability_path

```cpp
template<class T>
MaximumProbabilityPathResult<T> maximum_probability_path(
    const vector<vector<MaximumProbabilityPathEdge<T>>>& graph,
    int source
)
```

`source` から全頂点への最大成功確率と、その経路を復元するための親を求める。

## API別の時間計算量・空間計算量

$N=graph.size()$、$M$ を辺数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_probability_path(graph, source)` | $O((N+M)\log(N+1))$ | $O(N+M)$ |

## 注意点

- `source` と各辺の `to` は $[0,N)$ に含まれ、`probability` は $[0,1]$ でなければならない。違反時は `runtime_error` を送出する。
- `T` は $0,1$、比較、乗算を扱える型とする。浮動小数点型を使う場合、積には通常の丸め誤差が生じる。
- 確率が $1$ 以下なので、閉路を追加して成功確率が増えることはなく、最大値優先のDijkstra法が成立する。
- `parent` は同率の経路では更新しないため、同率の経路が複数ある場合の選択はグラフの格納順やpriority queueの順序に依存する。
