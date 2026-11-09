---
title: Upper Convex Hull (上側凸包)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/point_set/upper_convex_hull.hpp
---

## API

- `std::vector<Point> upper_convex_hull(std::vector<Point> points, bool keep_collinear = false)`: 凸包の上側の折れ線（上側チェーン）を作る。

## 引数

`points` は点集合、`keep_collinear` はチェーン上の中間の共線点を残すかどうかの指定である。

## 戻り値

$(x, y)$ の辞書順で最小の点から最大の点へ並ぶ上側チェーンを返す。

## API別の時間計算量・空間計算量

- `upper_convex_hull`: 点数を $N$ として時間 $O(N \log N)$、領域 $O(N)$。

## 注意点

- 座標が完全に一致する重複点は除く。
- 全点が共線で `keep_collinear = false` なら両端だけを返す。
