---
title: Closest Pair Indices from Integer Points (整数点列の最近点対添字)
documentation_of: ../../../../src/algorithm/geometry/2d/closest_pair_indices_points.hpp
---

整数座標点集合の最近点対を入力添字で求める。距離比較に浮動小数点演算を使わない。

## API

```cpp
template<std::integral Coordinate>
ClosestPairResult closest_pair_indices(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
);
```

平方距離が最小となる組を返す。同率なら
`(min(i, j), max(i, j))` が辞書順最小の組を選ぶ。重複座標の異なる点も区別する。

## 時間計算量

点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `closest_pair_indices(points)` | $O(N\log N)$ | $O(N)$ |

## 注意点

- `Coordinate` は符号付き・符号なしの64-bit以下の整数型でなければならない。
- 0点または1点では `exists()` がfalseの結果を返す。
- 座標差の二乗和は `ExactInteger` で誤差なく計算する。

## 使用例

```cpp
const std::vector<std::pair<long long, long long>> points{
    {0, 0}, {5, 0}, {1, 0},
};
const ClosestPairResult result = closest_pair_indices(points);
```
