---
title: 3D Point Set Diameter (三次元点集合直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/point_set_diameter_3d_default.hpp
---

厳密な AABB 分枝限定法により、三次元点集合のユークリッド直径（最遠点対）を求める。実装は `point_set_diameter_3d_aabb_branch_and_bound` をそのまま呼ぶ。

## API

- `point_set_diameter_3d(points)`: 最大距離と両端のインデックスを `std::optional<PointSetDiameter3DResult>` で返す。同距離の対が複数あれば、インデックス対が辞書順最小のものを選ぶ。点が 2 個未満なら `std::nullopt` を返す。

## API別の時間計算量・空間計算量

点数を $N$、AABB 木で訪れたノード対の数を $K$ とすると、構築は平均 $O(N \log N)$、探索は $O(K)$、追加空間は $O(N)$。通常は距離の上界で大部分のノード対を捨てられるが、最悪の場合は $K = O(N^2)$ になる。

## 注意点

- 座標は有限でなければならず、違反した場合は `std::invalid_argument` を送出する。
- 距離比較では座標差を仮数と 2 進指数に分けて扱い、オーバーフローを避ける。真の距離が `long double` の最大値を超える場合、返す `distance` は正の無限大になる。
- AABB の浮動小数点による上界は枝刈り候補の選別にだけ用い、判定が不確実な比較ではノード対を残すため、枝刈りによって正解を失うことはない。
