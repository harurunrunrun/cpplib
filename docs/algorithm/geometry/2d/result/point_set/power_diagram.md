---
title: Power Diagram (パワー図)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/power_diagram.hpp
---

重み付き二乗距離 $|x-p_i|^2-w_i$ の最小siteごとに、有界多角形内のcellを構築する。

## API

`power_diagram(sites, bounding_polygon)` は入力site順の `cells` を返す。空cellも保持する。

## 時間計算量

site数を $N$、境界頂点数を $B$ とすると $O(N^2(N+B))$ 時間、
$O(N(N+B))$ 出力空間。

## 注意点

境界は反時計回りの凸多角形とする。site座標と重みは有限値でなければならない。
