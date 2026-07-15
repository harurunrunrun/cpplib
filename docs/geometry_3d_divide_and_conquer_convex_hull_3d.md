---
title: Divide and Conquer Convex Hull 3D (三次元分割統治凸包)
documentation_of: ../src/algorithm/geometry/3d/divide_and_conquer_convex_hull_3d.hpp
---

## API

- `divide_and_conquer_convex_hull_3d(points)`: 辞書順に二分し、各部分凸包の頂点だけを次段のmerge候補に残して凸包を構成する。

## API別の時間計算量・空間計算量

- 増分mergeを用いるため最悪時間 $O(N^2\log N)$、再帰・候補列の領域 $O(N\log N)$。

## 注意点

重複点・全点一致・共線・共面を処理する。非有限座標には `std::invalid_argument` を送出する。
