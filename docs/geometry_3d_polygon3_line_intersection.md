---
title: Polygon3 Line Intersection (三次元多角形と直線の交差)
documentation_of: ../src/algorithm/geometry/3d/query/polygon3_line_intersection.hpp
---

## API

- `Polygon3LinearIntersection3`: 共通部分のうち、どの線分にも含まれない孤立交点を
  `points`、互いに内部が交わらない閉区間を `segments` に保持する。
  `empty()` は両方が空かを返す。
- `polygon3_line_intersection(polygon, line)`: 単純な平面多角形と直線の共通部分を返す。
  多角形は凸でなくてもよく、複数の区間を返せる。

## API別の時間計算量・空間計算量

頂点数を $N$、境界 event 数を $K$ とする。単純多角形では $K=O(N)$ である。

- 直線が多角形の平面を横切る場合は時間 $O(N)$、追加領域 $O(N)$。
- 直線が多角形の平面上にある場合は event と境界重複区間をソートするため、
  最悪時間 $O(N+K\log K)=O(N\log N)$、追加領域 $O(N+K)=O(N)$。
  各区間に対する多角形包含判定や、各点から全線分を走査する処理は行わない。
- `Polygon3LinearIntersection3::empty()` は時間・領域ともに $O(1)$。

## 注意点

3頂点未満、非共面な頂点列、退化直線には `std::invalid_argument` を送出する。
多角形の境界と直線の端点を共通部分に含める。自己交差多角形は受理しない。
