---
title: 3D Convex Hull Internal Detail (三次元凸包内部実装)
documentation_of: ../../../../src/algorithm/geometry/3d/convex_hull_3d_detail.hpp
---

公開leaf間で共有する三次元凸包の内部実装を保持する。

## API

- `convex_hull_3d_detail::build_with_seed(points, random_seed)`: 公開wrapperから呼ばれる増分凸包構築本体。
- その他の `convex_hull_3d_detail` 内の型と関数: 重複除去、退化次元処理、conflict graph更新、面の正規化を実装する内部要素。

## API別の時間計算量・空間計算量

入力点数を $N$ とする。アフィン次元2以下は $O(N\log N)$ 時間・$O(N)$ 追加領域。三次元では入力独立な乱択順で期待 $O(N\log N)$ 時間・$O(N)$ 領域、最悪 $O(N^2\log N)$ 時間・$O(N^2)$ 領域。

## 注意点

内部実装用headerであり、名前・引数・返り値の互換性は公開APIではない。利用側は `convex_hull_3d_default.hpp` または `convex_hull_3d_with_seed.hpp` をincludeする。
