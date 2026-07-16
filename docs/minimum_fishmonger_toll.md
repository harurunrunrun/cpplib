---
title: Minimum Fishmonger Toll (制限時間内経路の最小通行料) [FISHER]
documentation_of: ../src/algorithm/other/minimum_fishmonger_toll.hpp
---

時間制限内に頂点0から末尾頂点へ着く経路の最小通行料を返す。

## FishmongerRouteCost

```cpp
struct FishmongerRouteCost { int toll, travel_time; };
```

## minimum_fishmonger_toll

```cpp
FishmongerRouteCost minimum_fishmonger_toll(
    const vector<vector<int>>& travel_times,
    const vector<vector<int>>& tolls, int time_limit
);
```

同じ最小通行料なら移動時間が短いものを返し、到達不能なら `{-1,-1}` を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_fishmonger_toll` | $O(TN^2)$ | $O(TN)$ |

## 注意点

- 行列は同じ正方形サイズ、非対角移動時間は正、通行料は非負でなければならない。
