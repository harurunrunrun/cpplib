---
title: Minimum Enclosing Ellipse (最小包含楕円)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/minimum_enclosing_ellipse.hpp
---

Khachiyan の反復法により、点集合を含む面積最小の楕円を指定した許容誤差まで近似する。

## API

```cpp
MinimumEnclosingEllipseResult minimum_enclosing_ellipse(
    const std::vector<Point>& points,
    long double tolerance = 1.0e-4L,
    std::size_t maximum_iterations = 200000
);
```

返り値 `MinimumEnclosingEllipseResult` は中心 `center`、$2 \times 2$ の二次形式 `quadratic_form`、近似許容誤差 `approximation_tolerance`（`tolerance` の値）を持つ。中心を $c$、二次形式の行列を $A$ とすると、楕円は $\lbrace x : (x - c)^{\top} A (x - c) \le 1 \rbrace$ である。左辺の値は `evaluate(point)` で計算できる。

## 時間計算量

点数を $N$、実際の反復回数を $I$（$I \le$ `maximum_iterations`）とすると、時間計算量は $O(NI)$、空間計算量は $O(N)$ である。返却前の包含補正は $O(N)$ で行う。

## 注意点

- 返却前に全点での二次形式の最大値で `quadratic_form` を縮小するため、丸め誤差の範囲で全入力点を必ず包含する。
- 点が 3 個未満の場合、`tolerance` が正の有限値でない場合、`maximum_iterations == 0` の場合、座標が有限でない場合は `std::invalid_argument` を送出する。
- 反復回数が上限に達しても収束しない場合は `std::runtime_error` を送出する。
- 点集合が 2 次元的に退化している（全点が一直線上にあるなど）場合は `std::domain_error` を送出する。
