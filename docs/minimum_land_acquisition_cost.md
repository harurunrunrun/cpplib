---
title: Minimum Land Acquisition Cost (土地一括購入の最小費用) [ACQUIRE]
documentation_of: ../src/algorithm/other/minimum_land_acquisition_cost.hpp
---

複数の長方形をグループ単位で購入するときの最小総費用を返す。

# LandPlotDimensions

```cpp
struct LandPlotDimensions { long long width, height; };
```

回転できない土地の幅と高さを表す。

# minimum_land_acquisition_cost

```cpp
long long minimum_land_acquisition_cost(vector<LandPlotDimensions> plots);
```

各グループの費用を最大幅と最大高さの積とした最小総費用を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_land_acquisition_cost` | $O(N\log N)$ | $O(N)$ |

## 注意点

- 幅と高さは正でなければならず、違反時は `runtime_error` を送出する。
- 結果が `long long` を超える場合は `overflow_error` を送出する。
