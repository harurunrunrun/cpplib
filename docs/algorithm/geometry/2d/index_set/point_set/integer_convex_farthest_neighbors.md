---
title: Integer Convex Farthest Neighbors (整数凸多角形の全頂点の最遠頂点) [all_furthest_neighbors]
documentation_of: ../../../../../../src/algorithm/geometry/2d/index_set/point_set/integer_convex_farthest_neighbors.hpp
---

`integer_convex_farthest_neighbors(points)` は凸多角形の各入力位置に対する最遠の入力位置を返す。正規化で頂点を削除せず、時計回り・反時計回り、同一直線上の中間点、連続する重複頂点を扱う。距離の比較には符号付き128 bit整数を使う。周回列を二分し、互いに交わらない弧の間の全単調行列にSMAWKを適用する。

## 時間計算量

$O(N\log(N+1))$、空間 $O(N)$。

## 注意点

入力は凸多角形の弱凸な周回列であること。凸性の検査は行わない。座標は $[-2^{62},2^{62})$ に制限する。最遠頂点が複数あればそのいずれかを返す。空列・1点・2点も扱う。添字・頂点数は `int` で表せる範囲。
