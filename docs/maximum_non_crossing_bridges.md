---
title: Maximum Non-Crossing Bridges (最大非交差橋数) [BRIDGE]
documentation_of: ../src/algorithm/other/maximum_non_crossing_bridges.hpp
---

# maximum_non_crossing_bridges

```cpp
int maximum_non_crossing_bridges(
    const vector<int>& first_side,
    const vector<int>& second_side
)
```

## 引数

- `first_side[i]`, `second_side[i]`: 橋 `i` の両岸における端点座標。

## 戻り値

内部で交差しないように選べる橋の最大本数。同じ端点で接する橋は交差とみなさない。

## API別の時間計算量・空間計算量

橋の本数を $N$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_non_crossing_bridges` | $O(N\log(N+1))$ | $O(N)$ |

## 注意点

- 2つの配列長は等しくなければならない。違反時は `invalid_argument` を送出する。
