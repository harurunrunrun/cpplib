---
title: 3D Convex Hull Internal Detail (三次元凸包内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point_set/convex_hull_3d_detail.hpp
---

公開されている個別ヘッダが共有する、三次元凸包の内部実装である。

## API

- `convex_hull_3d_detail::build_with_seed(points, random_seed)`: 公開ラッパーから呼ばれる、増分法による凸包構築の本体。
- その他の `convex_hull_3d_detail` 内の型と関数: 重複除去、退化した次元の処理、conflict グラフの更新、面の正規化を実装する内部要素。

## API別の時間計算量・空間計算量

入力点数を $N$ とする。アフィン次元が 2 以下なら時間 $O(N\log N)$、追加領域 $O(N)$。三次元では、入力に依存しない乱択順で期待時間 $O(N\log N)$、領域 $O(N)$、最悪時間 $O(N^2\log N)$、領域 $O(N^2)$。

## 注意点

内部実装用のヘッダであり、名前・引数・返り値の互換性は公開 API として保証しない。利用側は `convex_hull_3d_default.hpp` または `convex_hull_3d_with_seed.hpp` を include すること。
