---
title: Closest Pair of Points (indices) (最近点対) [closest_pair]
documentation_of: ../src/algorithm/geometry/2d/closest_pair_indices.hpp
---

整数座標点集合の最近点対を入力添字で求める。距離の比較には平方距離を用い、
浮動小数点演算を行わない。

## `closest_pair_indices`

```cpp
ClosestPairResult result = closest_pair_indices(points);
```

`points`は`std::vector<std::pair<Coordinate, Coordinate>>`で、`Coordinate`は整数型とする。
距離が最小となる組が複数あれば、$(\min(i,j),\max(i,j))$が辞書順最小の組を返す。
同じ座標を持つ異なる入力点も区別する。

0点または1点では組が存在しない。64-bit以下の整数座標を対象とし、
座標差の二乗和は `ExactInteger` で正確に計算する。

## `ClosestPairResult`

```cpp
struct ClosestPairResult {
    std::size_t first;
    std::size_t second;
    ExactInteger squared_distance;
    bool exists() const;
};
```

`exists()`がfalseなら有効な組はない。組が存在するとき`first < second`である。

## API別の時間計算量・空間計算量

$N$を点数とする。

| API・操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `closest_pair_indices` | $O(N\log N)$ | $O(N)$ |
| `ClosestPairResult::exists` | $O(1)$ | $O(1)$ |
| 各memberの参照 | $O(1)$ | $O(1)$ |

## 注意点

`Coordinate`は符号付き・符号なしのどちらでもよいが、幅は64-bit以下とする。
`squared_distance`は任意精度整数 `ExactInteger` で、値は常に非負である。
64-bit座標の二次元平方距離の最大値もoverflowせず保持する。
