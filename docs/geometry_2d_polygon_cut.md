---
title: General Polygon Line Cut (一般多角形の直線切断)
documentation_of: ../src/algorithm/geometry/2d/polygon_cut.hpp
---

## API

`PolygonLineCutResult polygon_cut(polygon, line)`

## 引数

単純多角形の周回順頂点列と有向非退化直線を渡す。

## 戻り値

`left` は直線左閉半平面、`right` は右閉半平面との共通部分を、反時計回りの連結成分列 `vector<vector<Point>>` で返す。

## API別の時間計算量・空間計算量

- `polygon_cut`: 頂点数を $N$ として時間 $O(N^2)$、領域 $O(N)$。

## 注意点

concave polygonで共通部分が非連結になる場合も成分を分ける。自己交差多角形は受理しない。退化直線では `invalid_argument`。
切断後faceの向きは、原点移動後の座標規模の二乗をscaleとする符号付き面積で判定するため、微小だが非退化なfaceも保持する。
退化判定に該当しない範囲では、`line.b - line.a` を正の定数倍しても切断結果は変わらない。
