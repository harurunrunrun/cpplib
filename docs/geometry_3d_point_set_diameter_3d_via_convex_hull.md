---
title: 3D Point Set Diameter via Convex Hull (三次元凸包経由点集合直径)
documentation_of: ../src/algorithm/geometry/3d/scalar/point_set_diameter_3d_via_convex_hull.hpp
---

三次元点集合の凸包を先に構築して内点・面内部点・辺上内部点を除き、残った極点を
AABB分枝限定法で調べる。返す直径は厳密であり、近似は行わない。

## API

- `point_set_diameter_3d_via_convex_hull(points)`: 再現可能な固定seedで凸包を構築し、
  直径の `PointSetDiameter3DResult{first, second, distance}` を返す。
- `point_set_diameter_3d_via_convex_hull_with_seed(points, random_seed)`:
  凸包の増分順序を決めるseedを指定して同じ結果を返す。期待計算量を使う場合は、
  入力から独立に選んだseedを渡す。

どちらも `first < second` であり、同距離なら元の入力index対が辞書順最小のものを
返す。点が2個未満なら `std::nullopt`。

## API別の時間計算量・空間計算量

入力点数を $N$、座標が異なる凸包極点数を $H$、AABB探索で訪れたノード対数を $R$、
葉で実際に比較した極点対数を $C$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `point_set_diameter_3d_via_convex_hull(points)` | 固定seedの最悪 $O(N^2\log N+H^2)$。返却距離が有限で、増分順序が非敵対的なら平均 $O(N\log N+H\log H+R+C)$ | 最悪 $O(N^2+H)$、通常 $O(N+H)$ |
| `point_set_diameter_3d_via_convex_hull_with_seed(points, random_seed)` | 返却距離が有限かつseedが入力と独立なら期待 $O(N\log N+H\log H+R+C)$、最悪 $O(N^2\log N+H^2)$ | 期待 $O(N+H)$、最悪 $O(N^2+H)$ |

アフィン次元が2以下なら凸包構築はseedによらず $O(N\log N)$ であり、全体は平均
$O(N\log N+H\log H+R+C)$、最悪 $O(N\log N+H^2)$ となる。元index対応表の構築は
$O(N\log N)$ 時間・$O(N)$ 空間で、表の計算量に含まれる。

$R+C$ は点配置に依存し、最悪 $O(H^2)$ である。したがって、入力と独立なseedを
使っても、全体の期待計算量を無条件に $O(N\log N)$ とはしない。多数の内点に対して
$H\ll N$ となる入力では、全 $N$ 点をAABB探索する版より比較数を減らせる。

真の距離を `long double` で表せず返却距離が正の無限大になる場合、異なる距離が
同じ無限大へ丸められる。この場合だけ元APIと同じindex tieを保つため全 $N$ 点の
AABB探索へ戻る。その探索の訪問ノード対数・実比較点対数を $R_N,C_N$ とすると
追加時間は平均前処理 $O(N\log N)$ と $O(R_N+C_N)$、最悪 $O(N^2)$ であり、
表の最悪上界には含まれる。

## 注意点

- 凸包頂点は入力座標そのものなので、各座標に一致する最小の元indexへ対応付ける。
  その後、極点を元index順に並べてAABB探索するため、同距離時の辞書順規則も保たれる。
- 全点が同一座標の場合、凸包頂点は一点だけになるが、直径0の元index対
  `{0, 1}` を返す。
- 共線・共面入力でも、それぞれ線分端点・二次元凸包頂点だけを調べる。
- 座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- AABB探索の上界と距離は `std::hypot` を使い、平方距離のoverflowを避ける。
- 距離そのものが `long double` の最大値を越える場合も、全点fallbackによって
  全探索APIと同じindex対を返す。
