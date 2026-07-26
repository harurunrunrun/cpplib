---
title: 3D Point Set Diameter via Convex Hull Internal Detail (凸包経由三次元点集合直径内部実装)
documentation_of: ../../../../src/algorithm/geometry/3d/point_set_diameter_3d_via_convex_hull_detail.hpp
---

二つの公開leafが共有する凸包極点から元入力indexへの復元処理を保持する。

## API

- `point_set_diameter_3d_via_convex_hull_detail::recover_original_indices(points, hull)`:
  凸包頂点を最小の元indexへ対応付け、AABB直径結果を元indexへ戻す内部関数。

## API別の時間計算量・空間計算量

入力点数を $N$、凸包極点数を $H$、AABB探索量を $R+C$ とする。
対応表と整列は $O(N\log N+H\log H)$ 時間、$O(N+H)$ 領域。
直径探索は $O(R+C)$、最悪 $O(H^2)$。距離が無限大なら全 $N$ 点探索の
最悪 $O(N^2)$ が加わる。

## 注意点

内部実装用headerであり、名前・引数・返り値の互換性は公開APIではない。
利用側はdefaultまたはseed指定leaf、あるいは互換aggregatorをincludeする。
