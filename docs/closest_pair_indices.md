---
title: Closest Pair of Points (indices) (最近点対)
documentation_of: ../src/algorithm/geometry/closest_pair_indices.hpp
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

0点または1点では組が存在しない。座標差の二乗和が128-bit符号なし整数に収まる必要がある。
Library Checkerの$|x|,|y|\leq10^9$では常に満たされる。

## `ClosestPairResult`

```cpp
struct ClosestPairResult {
    std::size_t first;
    std::size_t second;
    __uint128_t squared_distance;
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

座標と中間演算は有限な `long double` の範囲に収まる必要がある。境界・退化判定には各APIで明記した許容誤差を用いる。
