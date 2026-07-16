---
title: Area of Union of Circles (円の和集合面積)
documentation_of: ../src/algorithm/geometry/2d/circle_union_area.hpp
---

複数の円板の和集合面積を求める。各円周について他の円板に覆われる角度区間を
mergeし、露出した円弧をGreenの定理
$A=\frac12\oint(x\,dy-y\,dx)$ で積分する。包含・重複円も扱う。

## `circle_union_area`

```cpp
long double circle_union_area(const std::vector<Circle>& circles)
```

`circles` が表す閉円板の和集合面積を返す。空vectorと半径0だけなら0。

- 時間計算量: 円数を $N$ として $O(N^2\log N)$
- 追加空間計算量: $O(N)$

## 注意点

各円は有限の中心座標と非負の有限半径を必要とする。違反時は
`invalid_argument`。接触・包含判定にはgeometry共通のscale付き許容誤差を使う。
極端に座標scaleが異なる入力では、面積を原点付近へ平行移動してから計算すると
数値誤差を抑えられる。

