---
title: 3D Point Set Diameter via Convex Hull (三次元凸包経由点集合直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/point_set_diameter_3d_via_convex_hull.hpp
---

三次元点集合の凸包を先に構築して、内点・面の内部の点・辺上の内部の点を除き、残った極点を AABB 分枝限定法で調べる。返す直径は厳密であり、近似は行わない。

このヘッダは `point_set_diameter_3d_via_convex_hull_default.hpp` と `point_set_diameter_3d_via_convex_hull_with_seed.hpp` をまとめて include する、後方互換のための集約ヘッダである。利用する経路が 1 つなら、対応する個別ヘッダを直接 include すればよい。

## API

```cpp
std::optional<PointSetDiameter3DResult> point_set_diameter_3d_via_convex_hull(const std::vector<Point3>& points);
std::optional<PointSetDiameter3DResult> point_set_diameter_3d_via_convex_hull_with_seed(
    const std::vector<Point3>& points,
    std::uint64_t random_seed
);
```

- `point_set_diameter_3d_via_convex_hull(points)`: 決定的な分割統治法（`convex_hull_3d`）で凸包を構築し、直径を `PointSetDiameter3DResult{first, second, distance}` で返す。
- `point_set_diameter_3d_via_convex_hull_with_seed(points, random_seed)`: 凸包の増分順序を決めるシードを指定して（`convex_hull_3d_with_seed`）、同じ結果を返す。期待計算量を利用する場合は、入力と独立に選んだシードを渡す。

どちらも `first < second` であり、距離が等しい対が複数あれば、元の入力における添字対が辞書順最小のものを返す。点が 2 個未満なら `std::nullopt` を返す。

## API 別の時間計算量・空間計算量

入力点数を $N$、座標が相異なる凸包の極点の数を $H$、AABB 探索で訪れたノード対の数を $R$、葉で実際に比較した極点対の数を $C$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `point_set_diameter_3d_via_convex_hull(points)` | 最悪 $O(N \log N + H \log H + R + C)$ | $O(N+H)$ |
| `point_set_diameter_3d_via_convex_hull_with_seed(points, random_seed)` | 返す距離が有限かつシードが入力と独立なら期待 $O(N \log N + H \log H + R + C)$、最悪 $O(N^2 \log N + H^2)$ | 期待 $O(N+H)$、最悪 $O(N^2+H)$ |

アフィン次元が 2 以下なら、凸包の構築はシードによらず $O(N \log N)$ であり、全体は平均 $O(N \log N + H \log H + R + C)$、最悪 $O(N \log N + H^2)$ となる。元の添字への対応表の構築は時間 $O(N \log N)$・空間 $O(N)$ で、表の計算量に含まれる。

$R+C$ は点の配置に依存し、最悪 $O(H^2)$ である。したがって、入力と独立なシードを使っても、全体の期待計算量を無条件に $O(N \log N)$ とはしない。内点が多く $H \ll N$ となる入力では、全 $N$ 点を AABB 探索する版より比較回数を減らせる。

真の距離を `long double` で表せず、返す距離が正の無限大になる場合は、異なる距離が同じ無限大に丸められる。この場合に限り、元の API と同じ添字のタイブレークを保つため、全 $N$ 点の AABB 探索に切り替える。その探索で訪れたノード対の数・実際に比較した点対の数を $R_N, C_N$ とすると、追加時間は平均前処理 $O(N \log N)$ と探索 $O(R_N + C_N)$、最悪 $O(N^2)$ であり、表の最悪計算量に含まれる。

## 注意点

- 凸包の頂点は入力座標そのものなので、各座標に一致する最小の元の添字へ対応付ける。その後、極点を元の添字順に並べて AABB 探索するため、距離が等しい場合の辞書順規則も保たれる。
- すべての点が同一座標の場合、凸包の頂点は 1 点だけになるが、直径 0 の元の添字対 `{0, 1}` を返す。
- 共線・共面な入力では、それぞれ線分の端点・二次元凸包の頂点だけを調べる。
- 座標は有限でなければならない。違反時は `std::invalid_argument` を送出する。
- AABB 探索の上界と距離の計算には `std::hypot` を使い、二乗距離のオーバーフローを避ける。
- 距離そのものが `long double` の最大値を超える場合も、全点探索への切り替えによって、全探索 API と同じ添字対を返す。
