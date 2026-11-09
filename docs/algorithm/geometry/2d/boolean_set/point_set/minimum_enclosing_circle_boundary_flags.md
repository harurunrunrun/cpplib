---
title: Minimum Enclosing Circle Boundary Flags (最小包含円境界判定列) [minimum_enclosing_circle]
documentation_of: ../../../../../../src/algorithm/geometry/2d/boolean_set/point_set/minimum_enclosing_circle_boundary_flags.hpp
---

点集合の最小包含円を求め、各入力点がその円周上にあるかを返す。

## minimum_enclosing_circle_boundary_flags

```cpp
std::vector<bool> minimum_enclosing_circle_boundary_flags(
    const std::vector<Point>& points,
    std::uint64_t seed = 0x9e3779b97f4a7c15ULL
)
```

`result[i]` は、`points[i]` が最小包含円の円周上にあるときに限り `true` である。
内部では `minimum_enclosing_circle(points, seed)` で円を求め、各点に `on_boundary` を適用する。
同じ入力と `seed` に対しては同じ構築過程をたどる。空集合には空列を返す。

## 時間計算量

$N$ を `points.size()` として、期待 $O(N)$、最悪 $O(N^3)$。
最小包含円の構築後に全点を判定する部分は $O(N)$ である。

## 空間計算量

シャッフル用の点列のコピーと戻り値を含めて $O(N)$。

## 注意点

- 座標と中間演算は有限な `long double` の範囲に収まる必要がある。円を表現できない場合は `std::overflow_error` を送出する。
- 円周上かどうかは、点と中心の距離と半径の差を、両者の大きい方をスケールとする相対許容誤差（`GEOMETRY_EPS` と機械イプシロンに比例）で判定する。
- 重複点を含んでもよい。同じ座標の要素もそれぞれ個別に判定する。
