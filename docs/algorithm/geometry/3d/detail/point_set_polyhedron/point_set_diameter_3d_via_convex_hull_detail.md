---
title: 3D Point Set Diameter via Convex Hull Internal Detail (凸包経由三次元点集合直径内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point_set_polyhedron/point_set_diameter_3d_via_convex_hull_detail.hpp
---

2 つの公開個別ヘッダが共有する、凸包の頂点から元の入力の添字へ戻す処理を実装する。

## API

- `point_set_diameter_3d_via_convex_hull_detail::recover_original_indices(points, hull)`: 凸包の各頂点を、座標が一致する元の入力の最小の添字へ対応付け、凸包頂点上での AABB 直径探索の結果を元の添字へ戻す内部関数。点が 2 個未満なら `std::nullopt` を返す。凸包が 1 点だけ（全点が一致）なら添字の組 $(0, 1)$ を返す。

## API別の時間計算量・空間計算量

入力点数を $N$、凸包の頂点数を $H$、AABB 探索の手間を $R+C$ とする。
対応表（`std::map`）の構築と整列は時間 $O(N\log N+H\log H)$、領域 $O(N+H)$。
直径の探索は $O(R+C)$ で、最悪 $O(H^2)$。距離が無限大にしか表現できない場合は、タイの規則を保つために全 $N$ 点で探索し直すので、最悪 $O(N^2)$ が加わる。

## 注意点

内部実装用のヘッダであり、名前・引数・返り値の互換性は公開 API として保証しない。
凸包の頂点が入力点と一致しないなど内部の不変条件が破れた場合は `std::logic_error` を送出する。
利用側はデフォルト版またはシード指定版の個別ヘッダ、あるいは互換用の集約ヘッダを include すること。
