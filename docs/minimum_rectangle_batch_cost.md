---
title: Minimum Rectangle Batch Cost (長方形バッチ分割の最小費用) [ACQUIRE]
documentation_of: ../src/algorithm/other/dynamic_programming/minimum_rectangle_batch_cost.hpp
---

長方形集合を複数のバッチへ分割し、各バッチの最大幅と最大高さの積をその費用としたときの総費用を最小化する。

## RectangleBatchDimensions

```cpp
struct RectangleBatchDimensions {
    long long width;
    long long height;
};
```

回転しない長方形の正の幅と高さを表す。

## minimum_rectangle_batch_cost

```cpp
long long minimum_rectangle_batch_cost(
    vector<RectangleBatchDimensions> rectangles
);
```

各長方形をちょうど1つのバッチに割り当てたときの最小総費用を返す。別の長方形に幅・高さの両方で包含される長方形と、同じ幅を持つ冗長な長方形は内部で除去する。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_rectangle_batch_cost` | $O(N\log N)$ | $O(N)$ |

## 注意点

- 幅と高さは正でなければならず、違反時は `runtime_error` を送出する。
- 空の入力には0を返す。
- 結果が `long long` を超える場合は `overflow_error` を送出する。
