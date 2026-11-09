---
title: Power Diagram (パワー図)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/power_diagram.hpp
---

重み付き平方距離 $\lVert x - p_i \rVert^2 - w_i$ を最小にする site ごとに、有界な凸多角形内のセルを構築する。

## API

```cpp
PowerDiagramResult power_diagram(
    const std::vector<WeightedPoint>& sites,
    std::vector<Point> bounding_polygon
);
```

`WeightedPoint` は点 `point` と重み `weight` を持つ。返り値の `cells` は入力の site 順に並んだセル（反時計回りの多角形）の列で、空のセルも保持する。各セルは境界多角形を他の各 site との半平面で順に切り取って求める。

## 時間計算量

site 数を $N$、境界多角形の頂点数を $B$ とすると、時間計算量は $O(N^2 (N + B))$、出力の空間計算量は $O(N (N + B))$ である。

## 注意点

- 境界は凸多角形とする。頂点の向きは時計回り・反時計回りのどちらでもよく、内部で反時計回りに正規化する。
- 正規化後の境界の頂点数が 3 未満なら `std::invalid_argument` を送出する。
- 境界の座標、site の座標と重みは有限値でなければならず、そうでなければ `std::invalid_argument` を送出する。
