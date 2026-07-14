---
title: Maximum Route Success Percent (最大経路成功率) [CHICAGO]
documentation_of: ../src/algorithm/graph/maximum_route_success_percent.hpp
---

# RouteSuccessPercentEdge

```cpp
struct RouteSuccessPercentEdge {
    int left;
    int right;
    int percent;
};
```

無向路と、その路を通過できる確率（百分率）を表す。

# maximum_route_success_percent

```cpp
double maximum_route_success_percent(
    int vertex_count,
    const vector<RouteSuccessPercentEdge>& edges
)
```

頂点0から頂点 `vertex_count - 1` へ到達する経路の成功率の最大値を百分率で返す。到達不能なら0を返す。

## API別の時間計算量・空間計算量

$N=vertex_count$, $M=edges.size()$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_route_success_percent` | $O((N+M)\log(N+1))$ | $O(N+M)$ |

## 注意点

- `vertex_count` は正、端点は $[0,N)$、`percent` は $[0,100]$ でなければならない。
- 戻り値は浮動小数点数であり、表示時の丸めは呼び出し側で指定する。
