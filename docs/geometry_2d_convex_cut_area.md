---
title: 2D Convex Cut Area (二次元凸多角形切断面積) [CGL_4_C]
documentation_of: ../src/algorithm/geometry/2d/convex_cut_area.hpp
---

## API

- <code>convex_cut_area(polygon, line)</code>: 有向直線 <code>line.a -&gt; line.b</code> の左側に残る凸多角形部分の非負面積を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間 $O(N)$、追加領域 $O(N)$。

## 注意点

<code>polygon</code> は頂点順の凸多角形でなければならない。時計回り・反時計回りのどちらでも非負面積を返す。境界上の点を残し、<code>line</code> が退化している場合は <code>std::invalid_argument</code> を送出する。
