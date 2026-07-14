---
title: Maximum Fun Walk (有向歩道最大頂点価値) [GOODA]
documentation_of: ../src/algorithm/graph/maximum_fun_walk.hpp
---

# maximum_fun_walk

```cpp
optional<long long> maximum_fun_walk(
    const vector<vector<int>>& graph,
    const vector<long long>& fun,
    int start,
    int goal
)
```

## 引数

- `graph`: 有向隣接リスト。
- `fun[v]`: 頂点 `v` を初めて訪れたときに得る非負価値。
- `start`, `goal`: 始点と終点。

## 戻り値

`start` から `goal` までのwalkで得られる頂点価値和の最大値。到達不能なら `nullopt`。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_fun_walk` | $O(N+M)$ | $O(N+M)$ |

## 注意点

- `fun.size() == graph.size()`、価値は非負、頂点番号は範囲内でなければならない。
- 前提違反または価値和のoverflowでは対応する標準例外を送出する。
