---
title: Enumerate Points on Polygon Boundary (多角形境界上の点列挙)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer_set/point_set_point_set/enumerate_points_on_polygon_boundary.hpp
---

## API

```cpp
std::vector<int> enumerate_points_on_polygon_boundary(
    const std::vector<Point>& polygon,
    const std::vector<Point>& points,
    bool unique = true
);
```

多角形の辺上にある `points` の添字を、辺の順に、同じ辺の上では辺の進行方向の順に返す。

`unique` が真なら各添字は最初に現れたときだけ返す。偽なら、頂点上の点などは接するすべての辺について重複して返す。

## API別の時間計算量・空間計算量

- 辺数を $N$、点数を $M$、出力長を $K$ として、時間 $O(NM + K \log (K + 1))$、追加領域 $O(M + K)$。

## 注意点

多角形は頂点順に与える。長さがちょうど 0 の辺にも対応する。長さが正の辺では、辺方向への射影を辺の長さの 2 乗で割ったパラメータで進行順を決め、同じパラメータなら添字の小さい順にする。
線分上かどうかの判定には、絶対距離 `GEOMETRY_EPS` と機械の丸め誤差を用いる。戻り値は座標ではなく、入力 `points` の添字である。
