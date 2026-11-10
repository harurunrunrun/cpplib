---
title: Minimum Rectangle Batch Cost (長方形バッチ分割の最小費用) [ACQUIRE]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/minimum_rectangle_batch_cost.hpp
---

長方形の集合を複数のバッチに分割し、各バッチの費用を「幅の最大値 $\times$ 高さの最大値」としたときの総費用を最小化する。

## RectangleBatchDimensions

```cpp
struct RectangleBatchDimensions {
    long long width;
    long long height;
};
```

回転しない長方形の幅と高さ（正の値）を表す。

## minimum_rectangle_batch_cost

```cpp
long long minimum_rectangle_batch_cost(
    std::vector<RectangleBatchDimensions> rectangles
);
```

各長方形をちょうど 1 つのバッチに割り当てたときの最小の総費用を返す。幅と高さの両方で別の長方形以下になる長方形（同じ幅で高さが小さいものを含む）は答えに影響しないので内部で除去し、残りを幅の昇順に並べて Convex Hull Trick で DP を計算する。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_rectangle_batch_cost` | $O(N\log N)$ | $O(N)$ |

## 注意点

- 幅と高さは正でなければならず、違反時は `std::runtime_error` を送出する。
- 空の入力には $0$ を返す。
- 結果が `long long` を超える場合は `std::overflow_error` を送出する。
