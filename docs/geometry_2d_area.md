---
title: 2D Polygon Area (二次元多角形面積) [CGL_3_A]
documentation_of: ../src/algorithm/geometry/2d/area.hpp
---

## API

- `area(polygon)`: 頂点列が囲む符号付き面積を返す。反時計回りなら正、時計回りなら負。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間 $O(N)$、追加領域 $O(1)$。

## 注意点

頂点列の順に辺を結んで末尾と先頭も結ぶ。単純多角形でない場合も代数的面積を返す。
