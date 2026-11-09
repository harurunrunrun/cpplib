---
title: Furthest Pair from Integer Points (整数点列の最遠点対)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/furthest_pair_points.hpp
---

整数座標の点集合の最遠点対を、凸包と rotating calipers で求める。

## API

```cpp
template<std::integral Coordinate>
FurthestPairResult furthest_pair(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
);
```

平方距離が最大となる添字の組を返す。返り値は `first < second` を満たす添字 `first`, `second` と、平方距離 `squared_distance`（`ExactInteger`）を持つ。同率の組が複数あるときは `(first, second)` が辞書順最小のものを選ぶ。座標が重複する異なる点も区別する。

## 時間計算量

点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `furthest_pair(points)` | $O(N \log N)$ | $O(N)$ |

## 注意点

- `Coordinate` は 64 bit 以下の整数型（符号の有無は問わない）でなければならない。
- 点が 0 個または 1 個のときは、`exists()` が `false` を返す結果を返す。
- 2 点以上がすべて同じ座標なら、添字の組 $(0, 1)$ と平方距離 $0$ を返す。
- 外積と平方距離は `ExactInteger` で誤差なく計算する。

## 使用例

```cpp
const std::vector<std::pair<long long, long long>> points{
    {0, 0}, {5, 0}, {1, 0},
};
const FurthestPairResult result = furthest_pair(points);
```
