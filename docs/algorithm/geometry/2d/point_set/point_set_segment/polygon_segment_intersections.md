---
title: Polygon-Segment Intersections (多角形と線分の交点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/point_set_segment/polygon_segment_intersections.hpp
---

## API

`std::vector<Point> polygon_segment_intersections(polygon, segment)`

## 引数

多角形の頂点を周回順に並べた列 `polygon` と、線分 `segment` を渡す。

## 戻り値

多角形の境界と線分の交点を、`segment.a` から `segment.b` へ向かう順に重複なく返す。辺と線分が重なる場合は、共通部分の両端点を含める。

## API別の時間計算量・空間計算量

- `polygon_segment_intersections`: 頂点数を $N$、境界の辺から得られる交点候補の個数を $K$ として、時間 $O(N + K \log K)$、追加領域 $O(K)$。重なり区間の端点も候補に加え、線分上のパラメータ順にソートしてから隣接する重複を除く。

## 注意点

退化線分（長さ $0$ の線分）では、その点が境界上にあれば 1 点を返す。
