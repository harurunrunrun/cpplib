---
title: 3D Point Set Diameter (三次元点集合直径)
documentation_of: ../src/algorithm/geometry/3d/point_set_diameter_3d.hpp
---

三次元点集合のEuclidean直径と、その両端のindexを正確に求める。
全点対を列挙する単純版と、入力の空間的な分離を利用する厳密なAABB分枝限定版を提供する。

## API

- `point_set_diameter_3d(points)`: 最大距離の
  `PointSetDiameter3DResult{first, second, distance}` を返す。`first < second` で、
  同距離ならindex対が辞書順最小のものを返す。点が2個未満なら `nullopt`。
- `point_set_diameter_3d_aabb_branch_and_bound(points)`: 各部分集合の軸平行境界箱から
  得られる距離上界で、直径を更新できない部分集合対を除外する。返り値と同距離時の
  規則は単純版と同じであり、近似は行わない。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `point_set_diameter_3d(points)` | $O(N^2)$ | $O(1)$ |
| `point_set_diameter_3d_aabb_branch_and_bound(points)` | 平均前処理 $O(N\log N)$ と探索 $O(R+C)$、最悪 $O(N^2)$ | $O(N)$ |

$R$ は訪れたAABBノード対の数、$C$ は葉で実際に比較した点対の数である。離れたclusterや
内点が多い入力では上界による枝刈りで $R+C$ が全点対数より大幅に小さくなる。一方、
球面上に多数の点がある場合などは二乗個の候補を除外できないことがあるため、最悪計算量を
$O(N\log N)$ とは主張しない。

既に凸包頂点列を持つ場合は、その $H$ 頂点だけを入力できる。単純版は $O(H^2)$、
AABB分枝限定版は入力依存で平均前処理 $O(H\log H)$ と探索 $O(R+C)$ になる。

## 注意点

- 3次元には2次元回転calipersの直接対応がない。AABB版は境界箱間で可能な最大距離が
  現在値未満のときだけ枝刈りする。
- 座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- 距離は3引数 `std::hypot` で計算し、平方距離のoverflowを避ける。
- AABB上界は丸め誤差で過小にならないよう正の方向へ広げてから比較する。
