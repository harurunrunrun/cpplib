---
title: 3D Point Set Diameter (三次元点集合直径)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/point_set_diameter_3d.hpp
---

三次元点集合のユークリッド直径と、その両端の添字を厳密に求める。入力の空間的な分離を利用する AABB 分枝限定法を用いる。

このヘッダは `point_set_diameter_3d_default.hpp` と `point_set_diameter_3d_aabb_branch_and_bound.hpp` をまとめて include する、後方互換のための集約ヘッダである。利用する経路が 1 つなら、対応する個別ヘッダを直接 include すればよい。

## API

```cpp
std::optional<PointSetDiameter3DResult> point_set_diameter_3d(const std::vector<Point3>& points);
std::optional<PointSetDiameter3DResult> point_set_diameter_3d_aabb_branch_and_bound(const std::vector<Point3>& points);
```

- `point_set_diameter_3d_aabb_branch_and_bound(points)`: 最大距離の点対を `PointSetDiameter3DResult{first, second, distance}`（`first < second`）で返す。距離が等しい対が複数あれば、添字対が辞書順最小のものを選ぶ。点が 2 個未満なら `std::nullopt` を返す。各部分集合の軸平行境界箱（AABB）から得られる距離の上界によって、直径を更新できない部分集合の対を除外する。近似は行わない。
- `point_set_diameter_3d(points)`: デフォルト API。現在の実装では `point_set_diameter_3d_aabb_branch_and_bound(points)` をそのまま呼ぶ（全点対を列挙する単純版ではない）。

## API 別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `point_set_diameter_3d(points)` | 平均前処理 $O(N \log N)$ と探索 $O(R+C)$、最悪 $O(N^2)$ | $O(N)$ |
| `point_set_diameter_3d_aabb_branch_and_bound(points)` | 平均前処理 $O(N \log N)$ と探索 $O(R+C)$、最悪 $O(N^2)$ | $O(N)$ |

$R$ は訪れた AABB ノード対の数、$C$ は葉で実際に比較した点対の数である。離れたクラスタや内点が多い入力では、上界による枝刈りで $R+C$ が全点対の数より大幅に小さくなる。一方、球面上に多数の点がある場合などは二乗個の候補を除外できないことがあるため、最悪計算量を $O(N \log N)$ とは主張しない。

すでに凸包の頂点列を持っている場合は、その $H$ 頂点だけを入力すればよい。このとき平均前処理 $O(H \log H)$ と探索 $O(R+C)$ になる。

## 注意点

- 三次元には二次元の回転キャリパー法に直接対応する手法がない。AABB 版は、境界箱の間で可能な最大距離が現在の値未満のときだけ枝刈りする。
- 座標は有限でなければならない。違反時は `std::invalid_argument` を送出する。
- 各座標の差を仮数と 2 進指数に分け、共通の指数に正規化してから `std::hypot` を使う。これにより、有限な座標同士の減算が先にオーバーフローして距離比較の情報を失うことを避ける。
- 真の距離を `long double` で表せない場合、返す `distance` は正の無限大になる。
- AABB による上界は、丸め誤差で過小にならないよう正の方向へ広げてから比較する。上界の各軸の差も仮数と指数のまま比較する。
