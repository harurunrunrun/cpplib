---
title: Exact Integer Convex Geometry (厳密整数凸幾何)
documentation_of: ../src/algorithm/integer_geometry/convex.hpp
---

## API

- `convex_hull(points)`: 重複点と辺途中の共線点を除いた凸包を反時計回りで返す。全点共線なら両端だけを返す。
- `is_convex(polygon, strict)`: 頂点順の多角形が凸かを返す。`strict=true` では一直線の連続三点を許さない。
- `add_points(a, b)`: 座標ごとの和を返す。`int64_t` を越えるなら `std::overflow_error` を送出する。
- `minkowski_sum_convex(a, b)`: 凸多角形二つのMinkowski和を返す。
- `minkowski_sum(a, b)`: 任意の二点集合の凸包同士のMinkowski和を返す。

## API別の時間計算量・空間計算量

- `convex_hull`: 点数を $N$ として時間 $O(N\log N)$、領域 $O(N)$。
- `is_convex`: 頂点数を $N$ として時間 $O(N)$、追加領域 $O(1)$。
- `add_points`: 時間・追加領域 $O(1)$。
- `minkowski_sum_convex`: 頂点数を $N,M$ として時間 $O(N+M)$、出力領域 $O(N+M)$。
- `minkowski_sum`: 入力点数を $N,M$ として時間 $O(N\log N+M\log M)$、領域 $O(N+M)$。

## 注意点

`is_convex` には自己交差しない頂点列を渡す。`minkowski_sum_convex` には重複頂点・辺途中の共線点を含まない凸多角形を巡回順で渡す。1点または2点の退化凸多角形も受理し、これらを含む場合も $O(N+M)$ で処理する。時計回り入力は面積総和を作らず、極値対応の厳密な向き判定で反転する。空集合との和は空集合になる。出力座標が `int64_t` で表せない場合は `std::overflow_error`、内部配列の大きさが `size_t` で表せない場合は `std::length_error` を送出する。
