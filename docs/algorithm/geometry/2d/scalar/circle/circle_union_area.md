---
title: Area of Union of Circles (円の和集合面積)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/circle/circle_union_area.hpp
---

複数の円板の和集合の面積を求める。各円周について、他の円板に覆われる角度区間をマージし、露出している円弧に沿って Green の定理 $A = \frac{1}{2} \oint (x \, dy - y \, dx)$ で積分する。包含される円や重複する円も扱う。

## API

```cpp
long double circle_union_area(const std::vector<Circle>& circles);
```

`circles` が表す閉円板の和集合の面積を返す。空の列や、半径 0 の円だけからなる列では 0 を返す。

- 時間計算量: 円の数を $N$ として $O(N^2 \log N)$
- 追加空間計算量: $O(N)$

## 注意点

- 各円の中心座標は有限、半径は非負の有限値でなければならず、違反すると `std::invalid_argument` を送出する。
- 接触・包含の判定には幾何ライブラリ共通のスケール付き許容誤差を用いる。
- 座標のスケールが極端に異なる入力では、全体を原点付近へ平行移動してから計算すると数値誤差を抑えられる。
