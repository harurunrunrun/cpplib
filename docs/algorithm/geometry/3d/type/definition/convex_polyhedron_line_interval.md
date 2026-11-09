---
title: Convex Polyhedron Line Interval (凸多面体と直線の交差区間)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/convex_polyhedron_line_interval.hpp
---

## API

- `convex_polyhedron_line_interval(polyhedron, line)`: 直線 $a + t(b - a)$ （$a, b$ は `line.a`, `line.b`）が立体に含まれるパラメータ $t$ の閉区間 $[t_{\min}, t_{\max}]$ を `std::optional<std::pair<long double, long double>>` で返す。交差しなければ `std::nullopt` を返す。

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として、時間 $O(V + F)$、追加領域 $O(1)$。

## 注意点

- 外向きの面を持つアフィン次元 $3$ の凸多面体と、退化していない直線を前提とする。
- `polyhedron.affine_dimension != 3` の場合、直線または頂点の座標が有限でない場合、直線が退化している（`line.a` と `line.b` が一致する）場合は `std::invalid_argument` を送出する。面の頂点インデックスが範囲外なら `std::out_of_range` を送出する。
- 全座標を最大絶対値で正規化してから差を計算する。面との平行判定や区間の空判定には、機械イプシロンに比例する許容誤差を用いる。
- 半直線版の `convex_polyhedron_ray_interval` も同じヘッダで定義される内部処理を共有する。
