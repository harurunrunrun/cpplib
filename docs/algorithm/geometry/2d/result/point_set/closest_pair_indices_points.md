---
title: Closest Pair Indices from Integer Points (整数点列の最近点対添字)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/closest_pair_indices_points.hpp
---

整数座標の点集合の最近点対を、入力の添字で求める。距離の比較に浮動小数点演算を使わない。

## API

```cpp
template<std::integral Coordinate>
ClosestPairResult closest_pair_indices(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
);
```

距離の二乗が最小となる組を返す。戻り値の `first < second` は入力の添字、`squared_distance` は距離の二乗（`ExactInteger`）である。距離が等しい組が複数あれば、`(first, second)` が辞書順で最小の組を選ぶ。座標が重複していても添字の異なる点は区別する（その場合の距離は $0$）。

## 時間計算量

点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `closest_pair_indices(points)` | $O(N \log N)$ | $O(N)$ |

## 注意点

- `Coordinate` は 64 ビット以下の符号付きまたは符号なし整数型でなければならない（`static_assert` で検査する）。
- 点が $0$ 個または $1$ 個なら、`exists()` が `false` の結果を返す。
- 座標差の二乗和は `ExactInteger` で誤差なく計算する。

## 使用例

```cpp
const std::vector<std::pair<long long, long long>> points{
    {0, 0}, {5, 0}, {1, 0},
};
const ClosestPairResult result = closest_pair_indices(points);
```
