---
title: 3D Welzl Minimum Sphere (三次元Welzl最小包含球)
documentation_of: ../src/algorithm/geometry/3d/minimum_enclosing_sphere_welzl.hpp
---

Welzlの再帰アルゴリズムで三次元点集合の最小包含球を求める。境界集合は高々4点で、
退化時は `minimum_enclosing_sphere` のsupport overloadが最小候補を選ぶ。

## API

- `minimum_enclosing_sphere_welzl(points, seed)`: `points` の最小包含球を返す。
  `seed` は再帰前のshuffleを再現する。

## API別の時間計算量・空間計算量

期待時間 $O(N)$、最悪時間 $O(N^4)$。入力copyと再帰stackに $O(N)$ 領域を使う。

## 注意点

- 空集合または非有限座標の扱いは `minimum_enclosing_sphere` と同じである。
- 再帰深さは $N$ になる。非常に大きい入力でstack制限が厳しい場合は、非再帰の
  `minimum_enclosing_sphere(points, seed)` を使う。
- 判定にはscale依存の浮動小数許容誤差を使う。
