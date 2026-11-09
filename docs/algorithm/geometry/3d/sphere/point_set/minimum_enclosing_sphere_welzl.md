---
title: 3D Welzl Minimum Sphere (三次元Welzl最小包含球)
documentation_of: ../../../../../../src/algorithm/geometry/3d/sphere/point_set/minimum_enclosing_sphere_welzl.hpp
---

Welzl の再帰アルゴリズムで三次元点集合の最小包含球を求める。境界集合は高々 4 点であり、退化している場合は `minimum_enclosing_sphere` の 1〜4 点版が候補の中から最小の球を選ぶ。

## API

- `minimum_enclosing_sphere_welzl(points, seed = 0xD1B54A32D192ED03ULL)`: `points`（`std::vector<Point3>`、値渡し）の最小包含球を返す。`seed` は再帰前のシャッフルに用いるシードで、同じシードなら同じ結果を再現する。

## API別の時間計算量・空間計算量

点数を $N$ とする。期待時間 $O(N)$、最悪時間 $O(N^4)$。入力のコピーと再帰のスタックに $O(N)$ の領域を使う。

## 注意点

- 空集合や非有限な座標の扱いは `minimum_enclosing_sphere` と同じである（`std::invalid_argument` を送出する）。
- 再帰の深さは $\Theta(N)$ になる。非常に大きな入力でスタックの制限が厳しい場合は、非再帰の `minimum_enclosing_sphere(points, seed)` を使う。
- 包含判定にはスケールに比例する浮動小数点の許容誤差を用いる。
