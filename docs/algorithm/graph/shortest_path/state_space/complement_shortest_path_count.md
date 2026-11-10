---
title: Complement Shortest Path Count (補グラフ最短路数) [ABC319_G]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/state_space/complement_shortest_path_count.hpp
---

無向単純グラフの辺集合を除外辺とし、その補グラフ上での最短路の本数を数える。

## `complement_shortest_path_count`

```cpp
long long complement_shortest_path_count(
    int vertex_count,
    const std::vector<std::pair<int, int>>& excluded_edges,
    int source,
    int target,
    long long modulus
)
```

`excluded_edges` に含まれない相異なる 2 頂点の間すべてに辺を張った無向の補グラフで、`source` から `target` への最短路の本数を `modulus` で割った余りを返す。到達不能なら `-1` を返す。同じ除外辺の重複と自己ループは結果に影響しない。`source == target` のときは長さ 0 の路を 1 本と数える。

補グラフ上の BFS で距離ごとの層を求め、各層について「前の層の本数の総和」から除外辺で隣接する前の層の頂点の本数を引いて数える。

## 時間計算量

$N$ を `vertex_count`、$M$ を `excluded_edges` の要素数として $O(N + M\log(M + 1))$（除外辺の隣接リストのソートを含む）。

## 空間計算量

除外辺の隣接リストと探索領域を含めて $O(N + M)$。

## 注意点

- `vertex_count > 0`、`modulus > 0` でなければならない。
- `source`、`target` と全除外辺の端点は $0$ 以上 `vertex_count` 未満でなければならない。
- これらの前提に違反すると `runtime_error` を送出する。
