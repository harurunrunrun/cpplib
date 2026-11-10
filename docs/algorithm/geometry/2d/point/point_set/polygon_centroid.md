---
title: 2D Polygon Centroid (二次元多角形の面積重心)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point/point_set/polygon_centroid.hpp
---

## API

`Point polygon_centroid(polygon)`

## 引数

単純多角形の頂点を周回順に並べた列 `polygon`（`std::vector<Point>`）を渡す。

## 戻り値

面密度が一様な多角形の重心。

## API別の時間計算量・空間計算量

- `polygon_centroid`: 頂点数を $N$ として時間 $O(N)$、追加領域 $O(1)$。

## 注意点

- 頂点数が $3$ 未満の場合、または符号付き面積が相対許容誤差の範囲で $0$ の場合は `std::invalid_argument` を送出する。
- 時計回りの入力にも対応する。
- 先頭頂点を原点とした三角形分割で面積と局所的な加重和を計算し、最後に先頭頂点の座標を足し戻す。これにより、大きな平行移動や、凹多角形で正負の面積が打ち消し合うことによる精度低下を抑える。
- 座標と結果は有限な `long double` の範囲に収まる必要がある。
