---
title: Euclidean MST Result (ユークリッド最小全域木の結果)
documentation_of: ../../../../src/algorithm/geometry/2d/euclidean_mst_result.hpp
---

ユークリッド最小全域木の辺列を保持する結果型。

## API

```cpp
struct EuclideanMstResult {
    std::vector<EuclideanMstEdge> edges;
};
```

`edges` は選択された木の辺である。既定構築では空となる。

## 時間計算量

入力点数を $N$ とする。

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 既定構築、`edges` の参照、`edges.size()` | $O(1)$ | $O(1)$ |
| 全辺の列挙 | $O(N)$ | $O(1)$ |

## 注意点

空入力では辺数0、$N\geq1$ では辺数 $N-1$ となる。同一座標の点も別頂点であり、
それらを結ぶ辺の `squared_distance` は0となり得る。
