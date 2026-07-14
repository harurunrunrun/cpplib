---
title: Complement Shortest Path Count (補グラフ最短路数) [ABC319_G]
documentation_of: ../src/algorithm/graph/complement_shortest_path_count.hpp
---

無向単純グラフの辺集合を除外辺として、その補グラフ上の最短路数を数える。

# `complement_shortest_path_count`

```cpp
long long complement_shortest_path_count(
    int vertex_count,
    const vector<pair<int, int>>& excluded_edges,
    int source,
    int target,
    long long modulus
)
```

`excluded_edges` に含まれない相異なる2頂点間を辺とする無向補グラフで、
`source` から `target` への最短路の本数を `modulus` で割った余りとして返す。
到達不能なら `-1` を返す。同じ除外辺の重複と自己loopは結果に影響しない。
`source == target` では長さ0の路を1本と数える。

## 時間計算量

`N = vertex_count`、`M = excluded_edges.size()` として
$O(N^2 + M\log(M+1))$。

## 空間計算量

除外隣接リストと探索領域を含めて $O(N+M)$。

## 注意点

- `vertex_count > 0`、`modulus > 0` が必要である。
- `source`、`target`、全除外辺の端点は `0` 以上 `vertex_count` 未満でなければ
  ならない。
- 前提違反では `runtime_error` を送出する。
