---
title: 3D Halfspace Intersection (三次元半空間交差)
documentation_of: ../src/algorithm/geometry/3d/shape/halfspace_intersection_3d.hpp
---

平面が表す閉半空間の共通部分を、点・平面の極双対と三次元凸包を用いて
有界凸多面体として列挙する。

## API

- `halfspace_intersection_3d_with_interior_point(halfspaces, interior)`:
  各 `Plane3{point, normal}` を
  $normal\cdot(x-point)\leq0$ と解釈する。`interior` が全制約を狭義に満たすとき、
  極双対凸包から交差の頂点を復元する。非有界なら `std::domain_error`、
  `interior` が狭義内点でなければ `std::invalid_argument` を送出する。
- `halfspace_intersection_3d(halfspaces)`: 内点を与えない汎用版。ランダム増分型の
  固定次元凸二次計画で最小ノルム実行可能点と狭義内点を求め、full-dimensionalな
  交差には上の極双対法を使う。空集合なら空の `ConvexPolyhedron3` を返し、
  非空かつ非有界なら `std::domain_error` を送出する。lower-dimensionalな交差と、
  中間射影を `long double` で表現できない場合だけ、境界面の組合せを調べる
  完全な退化処理へ移る。

狭義内点を $c$、第 $i$ 制約を
$a_i\cdot(x-c)\leq s_i$（$s_i>0$）とすると、双対点は
$q_i=a_i/s_i$ である。実装は $q_i$ を除算して作らず、同次座標
$(s_i,a_{ix},a_{iy},a_{iz})$ の行列式をexact dyadic整数で判定する。
双対凸包の各facetが主問題の頂点に対応し、原点が双対凸包の狭義内部にない場合は
主問題が非有界である。

## API別の時間計算量・空間計算量

制約数を $H$、返す頂点と三角形面の総数を $K$ とする。

- `halfspace_intersection_3d_with_interior_point`: ランダム挿入順序の
  conflict graph解析では期待時間 $O(H\log H+K)$、期待領域 $O(H+K)$。
- `halfspace_intersection_3d`: full-dimensionalな非空入力では、狭義内点探索を
  含めて期待時間 $O(H\log H+K)$、期待領域 $O(H+K)$。空集合、
  lower-dimensional、または表現不能な中間射影を経る退化処理は
  最悪時間 $O(H^4+K)$、領域 $O(H+K)$。

固定seedの双対凸包挿入順序に対する決定的最悪時間は
$O(H^2\log H+K)$、live conflict pairの領域は最悪 $O(H^2)$ になり得る。
内点探索も含む汎用API全体の決定的最悪時間は $O(H^4+K)$ である。
以上はexact整数演算を1回と数える算術演算量で、入力のbit長に応じた
多倍長整数演算時間は別途掛かる。

## 注意点

- 法線は非零かつ全座標が有限でなければならない。違反時は
  `std::invalid_argument` を送出する。
- `interior` も全座標が有限で、すべての半空間を許容誤差を越えて狭義に
  満たす必要がある。
- 空集合と非有界集合を区別する。非有界集合のrayを返すAPIではない。狭義内点を
  要求する高速APIには、空集合やlower-dimensionalな交差を渡せない。
- lower-dimensionalな有界交差も、3面交点として列挙した頂点から構成する。
- 制約は単位法線へ丸めずraw法線のまま保持する。実行可能性、双対向き、
  coplanarityはexact dyadic演算で判定するため、正の定数倍をしても半空間は
  変わらない。
- 復元する頂点または内点探索の必須中間値が `long double` で表現不能なら
  `std::overflow_error` を送出する。
