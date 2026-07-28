---
title: 3D Point Set Diameter Internal Detail (三次元点集合直径内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point_set/point_set_diameter_3d_detail.hpp
---

二つの公開leafが共有する安全な距離比較とAABB分枝限定木を保持する。

## API

- `point_set_diameter_3d_detail::scaled_euclidean_distance(left, right)`:
  座標差のoverflowを避けてEuclidean距離を求める内部関数。
- `point_set_diameter_3d_detail::AabbBranchAndBound`: 点集合を分割し、
  AABB上界を使って直径候補を探索する内部型。
- その他のnamespace内要素: 入力検査、距離順位比較、結果更新用の内部関数。

## API別の時間計算量・空間計算量

距離評価一回は $O(1)$。AABB構築は平均 $O(N\log N)$ 時間、
$O(N)$ 領域。探索は訪問ノード対数 $R$ と比較点対数 $C$ に対して
$O(R+C)$ 時間、最悪 $O(N^2)$。

## 注意点

内部実装用headerであり、名前・引数・返り値の互換性は公開APIではない。
利用側は二つの公開leafまたは互換aggregatorをincludeする。
