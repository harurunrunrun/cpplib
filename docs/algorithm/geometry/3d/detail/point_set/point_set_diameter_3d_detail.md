---
title: 3D Point Set Diameter Internal Detail (三次元点集合直径内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point_set/point_set_diameter_3d_detail.hpp
---

2 つの公開個別ヘッダが共有する、安全な距離比較と AABB による分枝限定木を実装する。

## API

- `point_set_diameter_3d_detail::scaled_euclidean_distance(left, right)`: 座標差のオーバーフローを避けて Euclid 距離を求める内部関数。
- `point_set_diameter_3d_detail::AabbBranchAndBound`: 点集合を再帰的に分割し、AABB 間の距離の上界を使って直径の候補を探索する内部型。
- その他の名前空間内の要素: 入力検査（非有限な点には `std::invalid_argument`）、距離の大小比較、結果の更新のための内部関数。

## API別の時間計算量・空間計算量

距離の評価 1 回は $O(1)$。AABB 木の構築は中央値分割（`std::nth_element`）により期待時間 $O(N\log N)$、領域 $O(N)$。
探索は、訪問するノード対の数 $R$ と比較する点対の数 $C$ に対して時間 $O(R+C)$ で、最悪 $O(N^2)$ である。

## 注意点

内部実装用のヘッダであり、名前・引数・返り値の互換性は公開 API として保証しない。
利用側は 2 つの公開個別ヘッダ、または互換用の集約ヘッダを include すること。
