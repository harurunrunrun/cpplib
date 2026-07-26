---
title: Furthest Pair from Integer Points (整数点列の最遠点対)
documentation_of: ../../../../src/algorithm/geometry/2d/furthest_pair_points.hpp
---

整数座標点集合の最遠点対を、凸包とrotating calipersで求める。

## API

```cpp
template<std::integral Coordinate>
FurthestPairResult furthest_pair(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
);
```

平方距離が最大となる組を返す。同率なら
`(min(i, j), max(i, j))` が辞書順最小の組を選ぶ。重複座標の異なる点も区別する。

## 時間計算量

点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `furthest_pair(points)` | $O(N\log N)$ | $O(N)$ |

## 注意点

- `Coordinate` は符号付き・符号なしの64-bit以下の整数型でなければならない。
- 0点または1点では `exists()` がfalseの結果を返す。
- 2点以上がすべて同じ座標なら添字対 `(0, 1)` を返す。
- 外積と平方距離は `ExactInteger` で誤差なく計算する。

## 使用例

```cpp
const std::vector<std::pair<long long, long long>> points{
    {0, 0}, {5, 0}, {1, 0},
};
const FurthestPairResult result = furthest_pair(points);
```
