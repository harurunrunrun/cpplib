---
title: Maximum Probability Path (最大確率路) [CHICAGO]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/state_space/maximum_probability_path.hpp
---

各有向辺が $[0, 1]$ の値をもつグラフで、始点から各頂点へ至る路上の辺の値の積を最大化する。辺の値が $1$ 以下なので、最大値を優先する Dijkstra 法として計算できる。

## `ProbabilityEdge<T>`

```cpp
template <class T>
struct ProbabilityEdge {
    int to;
    T probability;
};
```

`to` を終点、`probability` を辺の値とする有向辺を表す。無向辺は両方向の辺として格納する。

## `MaximumProbabilityPathResult<T>`

```cpp
template <class T>
struct MaximumProbabilityPathResult {
    std::vector<T> probability;
    std::vector<int> parent;
    std::vector<char> reachable;
};
```

- `probability[v]`: 始点から `v` までの積の最大値。到達不能なら $0$。
- `parent[v]`: 最大値を最後に更新した直前の頂点。始点と到達不能な頂点では `-1`。
- `reachable[v]`: 始点から `v` へ到達できるとき真。積が $0$ になる到達可能な路と到達不能とを区別できる。

## `maximum_probability_path`

```cpp
template <class T>
MaximumProbabilityPathResult<T> maximum_probability_path(
    const std::vector<std::vector<ProbabilityEdge<T>>>& graph,
    int source
);
```

`source` から全頂点への積の最大値と、経路復元用の親を返す。

## 計算量

$N$ を `graph` の要素数、$M$ を格納した有向辺の本数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_probability_path(graph, source)` | $O((N + M)\log(N + M + 1))$ | $O(N + M)$ |

## 注意点

- `source` と各辺の `to` は $[0, N)$ の範囲内、`probability` は $[0, 1]$ の範囲内でなければならない。違反時は `runtime_error` を送出する。
- `T` は `T(0)`, `T(1)` の構築、比較、乗算ができる型とする。
- 浮動小数点型では積に丸め誤差が生じる。
- 同じ最大値を与える路が複数ある場合、`parent` がどれを指すかは辺の格納順などに依存する。
