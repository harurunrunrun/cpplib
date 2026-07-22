---
title: Minimum Enclosing Circle Boundary Flags (最小包含円境界判定列) [minimum_enclosing_circle]
documentation_of: ../src/algorithm/geometry/2d/query/minimum_enclosing_circle_boundary_flags.hpp
---

点集合の最小包含円を求め、各入力点がその円周上にあるかを返す。

## minimum_enclosing_circle_boundary_flags

```cpp
vector<bool> minimum_enclosing_circle_boundary_flags(
    const vector<Point>& points,
    uint64_t seed = 0x9e3779b97f4a7c15ULL
)
```

`result[i]` は `points[i]` が最小包含円の円周上にある場合に限り `true`。
同じ入力と `seed` には同じ構築過程を使用する。空集合には空列を返す。

## 時間計算量

`N = points.size()` として期待 $O(N)$、最悪 $O(N^3)$。
最小包含円の構築後の全点判定は $O(N)$。

## 空間計算量

shuffle用の点列と戻り値を含めて $O(N)$。

## 注意点

- 座標と中間演算は有限な `long double` の範囲に収まる必要がある。
- 円周判定には `geometry_sign` と `GEOMETRY_EPS` の許容誤差を使う。
- 重複点を含めてもよく、同じ座標の各要素を個別に判定する。
