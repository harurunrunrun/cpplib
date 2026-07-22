---
title: Convex Hull Detail (凸包内部処理)
documentation_of: ../src/algorithm/geometry/2d/detail/convex_hull_construction_detail.hpp
---

## API

- `convex_hull_detail::sorted_unique_points(points)`: 点を辞書順に整列し、重複を除く。
- `build_chain_from_sorted(points, upper, keep_collinear)`: 整列済み点列から上側または下側chainを作る。
- `build_convex_hull(points, keep_collinear)`: Andrew法で凸包を作る。

## 引数

`upper` は上側chainを選び、`keep_collinear` は辺上の中間点を残す。

## 戻り値

辞書順整列済み点列、左端から右端へのchain、または反時計回りの凸包頂点列を返す。

## API別の時間計算量・空間計算量

- `sorted_unique_points`, `build_convex_hull`: 点数を $N$ として時間 $O(N\log N)$、領域 $O(N)$。
- `build_chain_from_sorted`: 時間 $O(N)$、領域 $O(N)$。

## 注意点

これらは凸包API間で実装を共有する内部関数である。向き判定は2辺の長さの積で正規化した角度基準で行い、座標の正の一様scaleに依存しない。
