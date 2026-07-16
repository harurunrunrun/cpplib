---
title: Minimum Spanning Tree Cost (最小全域木コスト) [BLINNET]
documentation_of: ../src/algorithm/graph/minimum_spanning_tree_cost.hpp
---

重み付き無向グラフの最小全域木の重み総和を返す。

## `KruskalEdge<long long>`

```cpp
template<class T>
struct KruskalEdge {
    int from;
    int to;
    T cost;
};
```

辺の両端と重みを表す。`minimum_spanning_tree_cost` では `T = long long` を使用する。

## minimum_spanning_tree_cost

```cpp
long long minimum_spanning_tree_cost(
    int vertex_count,
    const vector<KruskalEdge<long long>>& edges
);
```

## 引数

- `vertex_count`: 頂点数。
- `edges`: 重み付き無向辺列。負の重みも許す。

## 戻り値

最小全域木の重み総和。`vertex_count` が0または1なら0を返す。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_spanning_tree_cost` | $O(N+M\log(M+1))$ | $O(N+M)$ |

## 注意点

- `vertex_count` は非負、各端点は $[0,N)$ でなければならない。範囲違反時は `runtime_error` を送出する。
- 全頂点を結ぶ全域木が存在しない場合は `invalid_argument` を送出する。
- 総和は内部で符号付き128 bit整数に拡張する。`long long` に収まらない場合は `overflow_error` を送出する。
