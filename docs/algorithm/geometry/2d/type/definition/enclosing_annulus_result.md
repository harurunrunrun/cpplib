---
title: Enclosing Annulus Result (包含円環結果型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/enclosing_annulus_result.hpp
---

点集合を包含する円環（中心 `center`、内半径 `inner_radius`、外半径 `outer_radius`）を格納する。

## API

```cpp
struct EnclosingAnnulusResult {
    Point center{};
    long double inner_radius = 0.0L;
    long double outer_radius = 0.0L;
};
```

3 つの公開メンバを直接参照する。デフォルト構築ではすべて $0$ となる。

## 時間計算量

すべてのメンバの参照は時間 $O(1)$、空間 $O(1)$。

## 注意点

半径は非負である。表す円環は境界を含む閉集合で、中心からの距離がちょうど内半径の点は穴に含まれない（円環に含まれる）。
