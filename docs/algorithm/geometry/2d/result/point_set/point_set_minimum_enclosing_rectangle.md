---
title: Point-Set Minimum Enclosing Rectangle (一般点集合の最小外接長方形)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/point_set_minimum_enclosing_rectangle.hpp
---

凸性を仮定しない点集合、または非凸多角形を含む面積最小の長方形を求める。

## API

```cpp
MinimumEnclosingRectangleResult point_set_minimum_enclosing_rectangle(
    std::vector<Point> points
);
```

凸包を構築してrotating calipersを適用する。結果の `corners`, `width`, `height`,
`area`, `valid` は `MinimumEnclosingRectangleResult` に格納される。

## 時間計算量

入力点数を $N$、凸包頂点数を $H$ とする。

- 時間計算量: $O(N\log(N+1)+H)$
- 追加空間計算量: $O(N)$

## 注意点

入力順と凸性を問わない。空入力では `valid == false` を返す。複数の最適解がある場合は
そのうち1つを返す。
