---
title: 3D Minimum Enclosing Sphere (三次元最小包含球)
documentation_of: ../src/algorithm/geometry/3d/minimum_enclosing_sphere.hpp
---

有限な三次元点集合をすべて含む半径最小の閉球を求める。1〜4点のoverloadは
support球を定数時間で求め、vector overloadはそれらを使う乱択増分法を実行する。

## API

- `minimum_enclosing_sphere(point)`: 1点の最小包含球を返す。
- `minimum_enclosing_sphere(a, b)`: 2点の最小包含球を返す。
- `minimum_enclosing_sphere(a, b, c)`: 3点の最小包含球を返す。
- `minimum_enclosing_sphere(a, b, c, d)`: 4点の最小包含球を返す。
- `minimum_enclosing_sphere(points, seed)`: `points` 全体の最小包含球を返す。
  `seed` は処理順のshuffleを再現するために使う。

support overloadは、全ての1〜4点部分集合が定める有限な候補球を列挙し、入力を
含む最小候補を選ぶ。このため重複、共線、共面、鈍角三角形を個別分岐なしで扱う。

## API別の時間計算量・空間計算量

- 1〜4点overload: 時間・追加領域 $O(1)$。
- vector overload: shuffle後の期待時間 $O(N)$、最悪時間 $O(N^4)$、入力copyを
  含む追加領域 $O(N)$。

## 注意点

- 空集合には有限で一意な最小球を定めず、`std::invalid_argument` を送出する。
- 座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- 有限な候補を構成できないほど中間値がoverflowした場合は
  `std::overflow_error` を送出する。
- 点差を局所scaleで正規化してから候補球を構成する。したがって、反対符号の巨大座標の
  差が直接表現できない場合や、非常に小さい点集合も扱える。真の有限な中心または半径を
  `long double`で表現できない場合は`std::overflow_error`を送出する。
- 所属判定の許容誤差は球と点距離の局所scaleに対する相対誤差であり、絶対誤差によって
  微小な点集合を半径0とみなすことはない。
