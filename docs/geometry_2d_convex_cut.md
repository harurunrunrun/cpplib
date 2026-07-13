---
title: 2D Convex Polygon Cut (二次元凸多角形切断)
documentation_of: ../src/algorithm/geometry/2d/convex_cut.hpp
---

## API

- `convex_cut(polygon, line)`: 有向直線 `line.a -> line.b` の左側にある凸多角形部分を頂点列で返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間 $O(N)$、出力・追加領域 $O(N)$。

## 注意点

境界上の点を残す。入力は頂点順の凸多角形、`line` は非退化直線でなければならない。
