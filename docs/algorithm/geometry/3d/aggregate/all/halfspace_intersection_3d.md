---
title: 3D Halfspace Intersection (三次元半空間交差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/halfspace_intersection_3d.hpp
---

平面が表す閉半空間の共通部分を、点と平面の極双対と三次元凸包を用いて、有界な凸多面体として列挙する。

このヘッダは `halfspace_intersection_3d_default.hpp` と `halfspace_intersection_3d_with_interior_point.hpp` をまとめて include する、後方互換のための集約ヘッダである。利用する経路が 1 つなら、対応する個別ヘッダを直接 include すればよい。

## API

- `halfspace_intersection_3d_with_interior_point(halfspaces, interior)`: 各 `Plane3{point, normal}` を半空間 $n \cdot (x - p) \le 0$（$p$ は `point`、$n$ は `normal`）と解釈する。`interior` がすべての制約を狭義に満たすとき、極双対凸包から共通部分の頂点を復元する。共通部分が非有界なら `std::domain_error`、`interior` が狭義内点でなければ `std::invalid_argument` を送出する。
- `halfspace_intersection_3d(halfspaces)`: 内点を与えない汎用版。ランダム増分型の固定次元凸二次計画で最小ノルムの実行可能点と狭義内点を求め、共通部分が三次元なら上の極双対法を使う。空集合なら空の `ConvexPolyhedron3` を返し、空でなく非有界なら `std::domain_error` を送出する。共通部分が低次元の場合と、途中の射影を `long double` で表現できない場合に限り、境界面の組合せを調べる完全な退化処理に移る。

狭義内点を $c$、第 $i$ 制約を $a_i \cdot (x - c) \le s_i$（$s_i > 0$）とすると、双対点は $q_i = a_i / s_i$ である。実装では $q_i$ を除算で作らず、同次座標 $(s_i, a_{ix}, a_{iy}, a_{iz})$ の行列式を厳密な 2 進有理数の整数演算で判定する。双対凸包の各 facet が主問題の頂点に対応し、原点が双対凸包の狭義内部にない場合は主問題が非有界である。

## API 別の時間計算量・空間計算量

制約数を $H$、返す頂点と三角形面の総数を $K$ とする。

- `halfspace_intersection_3d_with_interior_point`: ランダムな挿入順序に対する conflict graph の解析により、期待時間 $O(H \log H + K)$、期待空間 $O(H + K)$。
- `halfspace_intersection_3d`: 共通部分が三次元で空でない入力では、狭義内点の探索を含めて期待時間 $O(H \log H + K)$、期待空間 $O(H + K)$。空集合・低次元、または表現できない途中の射影を経る退化処理では、最悪時間 $O(H^4 + K)$、空間 $O(H + K)$。

双対凸包の挿入順序を固定シードで決めた場合の決定的な最悪時間は $O(H^2 \log H + K)$ であり、生存している conflict 対の空間は最悪 $O(H^2)$ になり得る。内点探索を含む汎用 API 全体の決定的な最悪時間は $O(H^4 + K)$ である。以上は厳密な整数演算を 1 回と数えた算術演算量であり、入力のビット長に応じた多倍長整数演算の時間が別途かかる。

## 注意点

- 法線は非零で、すべての座標が有限でなければならない。違反時は `std::invalid_argument` を送出する。
- `interior` もすべての座標が有限で、すべての半空間を許容誤差を超えて狭義に満たす必要がある。
- 空集合と非有界集合を区別する。非有界集合の半直線を返す API ではない。狭義内点を要求する高速 API には、空集合や低次元の共通部分を渡せない。
- 低次元の有界な共通部分も、3 平面の交点として列挙した頂点から構成する。
- 制約は単位法線に丸めず、元の法線のまま保持する。実行可能性、双対の向き、共面性は厳密な 2 進有理数演算で判定するため、制約を正の定数倍しても半空間は変わらない。
- 復元する頂点、または内点探索に必須の途中の値が `long double` で表現できなければ `std::overflow_error` を送出する。
