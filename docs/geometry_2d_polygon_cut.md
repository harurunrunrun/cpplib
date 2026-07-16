---
title: General Polygon Line Cut (一般多角形の直線切断)
documentation_of: ../src/algorithm/geometry/2d/polygon_cut.hpp
---

## API

```cpp
PolygonLineCutResult polygon_cut(
    const vector<Point>& polygon,
    const Line& line
)
```

## 引数

`polygon` には単純多角形の周回順頂点列、`line` には向きを持つ非退化直線を渡す。
多角形は凸でなくてもよい。

## 戻り値

`left` は直線の左閉半平面、`right` は右閉半平面との共通部分である。
それぞれを反時計回りの連結成分列 `vector<vector<Point>>` として返すため、
切断後の共通部分が非連結でも全成分を保持する。

## API別の時間計算量・空間計算量

頂点数を $N$ として、最悪時間 $O(N\log N)$、戻り値を含まない追加領域 $O(N)$。
境界辺の登録には ordered set、切断直線上の交点の整列にはソートを使う。
直線上の区間が多角形内部かどうかは、境界の上下関係から得た偶奇を一度の走査で更新し、
各区間に対する $O(N)$ の点包含判定は行わない。

## 注意点

自己交差や自己接触を持つ頂点列は受理しない。退化直線には
`std::invalid_argument` を送出する。切断後の面の向きは、原点移動後の座標規模の
二乗を scale とする符号付き面積で判定するため、微小でも非退化な面を保持する。
退化判定に該当しない範囲では、`line.b - line.a` を正の定数倍しても結果は変わらない。
