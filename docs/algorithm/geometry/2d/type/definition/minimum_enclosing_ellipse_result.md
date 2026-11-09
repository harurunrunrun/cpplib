---
title: Minimum Enclosing Ellipse Result (最小包含楕円結果型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/minimum_enclosing_ellipse_result.hpp
---

楕円の中心と、楕円を表す対称な二次形式を格納する。

## API

```cpp
struct MinimumEnclosingEllipseResult {
    Point center{};
    std::array<std::array<long double, 2>, 2> quadratic_form;
    long double approximation_tolerance = 0.0L;
    long double evaluate(const Point& point) const;
};
```

- `center`: 楕円の中心 $c$。
- `quadratic_form`: $2 \times 2$ の対称行列 $Q$。楕円は $\lbrace p \mid (p-c)^{\top} Q (p-c) \le 1 \rbrace$ である。
- `evaluate(point)`: 中心との差 $d = p - c$ に二次形式を適用した値 $d^{\top} Q d$ を返す。$1$ 以下なら楕円の内部（境界を含む）にある。
- `approximation_tolerance`: 構築時の反復の停止許容誤差。

デフォルト構築では中心が原点、二次形式と許容誤差が $0$ となる。

## 時間計算量

`evaluate` と全メンバの参照は時間 $O(1)$、空間 $O(1)$。
