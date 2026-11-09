---
title: Polygon3 Line Intersection (三次元多角形と直線の交差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polygon_line/polygon3_line_intersection.hpp
---

三次元空間内の平面多角形と直線の共通部分を求める。

## API

- `polygon3_line_intersection(polygon, line)`: 単純な平面多角形（境界を含む閉領域）と直線の共通部分を `Polygon3LinearIntersection3{points, segments}` で返す。多角形は凸でなくてもよく、共通部分が複数の線分や孤立点からなる場合はそれらをすべて返す。戻り値の `empty()` で共通部分が空か判定できる。

## API別の時間計算量・空間計算量

頂点数を $N$、境界上のイベント数を $K$ とする。単純多角形では $K = O(N)$ である。

- 直線が多角形の平面を横切る場合: 時間 $O(N)$、追加空間 $O(N)$。
- 直線が多角形の平面上にある場合: イベントと境界上の重複区間をソートするため、最悪時間 $O(N + K \log K) = O(N \log N)$、追加空間 $O(N + K) = O(N)$。区間ごとの多角形包含判定や、各点から全辺を走査する処理は行わない。

## 注意点

- 頂点数 3 未満または頂点が共面でない多角形、退化した直線（2 点の距離が `GEOMETRY3D_EPS` 以下）には `std::invalid_argument` を送出する。
- 多角形の境界も共通部分に含める。
- 自己交差する多角形は対象外である（入力の単純性は検査しない）。
