---
title: Point-Set Minimum Enclosing Rectangle (一般点集合の最小外接長方形)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/point_set_minimum_enclosing_rectangle.hpp
---

凸性を仮定しない点集合（非凸多角形の頂点列を含む）を含む、面積最小の長方形を求める。

## API

```cpp
MinimumEnclosingRectangleResult point_set_minimum_enclosing_rectangle(
    std::vector<Point> points
);
```

凸包を構築してから rotating calipers を適用する。結果の `corners`, `width`, `height`, `area`, `valid` は `MinimumEnclosingRectangleResult` に格納される。

## 時間計算量

入力点数を $N$ とする。

- 時間計算量: $O(N \log N)$（凸包の構築が支配的で、その後の rotating calipers は凸包の頂点数に対して線形）
- 追加空間計算量: $O(N)$

## 注意点

入力の順序と凸性は問わない。空入力では `valid == false` を返す。最適解が複数ある場合は、そのうちの 1 つを返す。
