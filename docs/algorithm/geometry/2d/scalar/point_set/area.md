---
title: 2D Polygon Area (二次元多角形面積) [CGL_3_A]
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set/area.hpp
---

頂点列が囲む多角形の符号付き面積を求める。

## API

```cpp
long double area(const std::vector<Point>& polygon);
```

頂点が反時計回りなら正、時計回りなら負の面積を返す。

## 時間計算量

頂点数を $N$ として、時間計算量 $O(N)$、追加領域 $O(1)$。

## 注意点

頂点列の順に辺を結び、末尾と先頭も結ぶ。単純多角形でない場合も、各辺の外積の和による代数的な面積を返す。
