---
title: 2D Convex Cut Area (二次元凸多角形切断面積) [CGL_4_C]
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set_line/convex_cut_area.hpp
---

凸多角形を有向直線で切断したとき、直線の左側に残る部分の面積を求める。

## API

```cpp
long double convex_cut_area(
    const std::vector<Point>& polygon,
    const Line& line
);
```

有向直線 `line`（`line.a` から `line.b` への向き）の左側に残る凸多角形の部分の、非負の面積を返す。

## 時間計算量

頂点数を $N$ として、時間計算量 $O(N)$、追加領域 $O(N)$。

## 注意点

`polygon` は頂点を周回順に並べた凸多角形でなければならない。時計回り・反時計回りのどちらでも非負の面積を返す。直線上の点は残す側に含める。`line` が退化している場合は `std::invalid_argument` を送出する。
