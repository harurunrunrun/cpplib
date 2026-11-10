---
title: Triangle Area from Three Sides (三辺から求める三角形面積)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/scalar/triangle_area_from_sides.hpp
---

3 辺の長さから三角形の面積を求める。

## API

```cpp
long double triangle_area_from_sides(
    long double first,
    long double second,
    long double third
);
```

## 引数

有限かつ非負の 3 辺の長さを渡す。順序は問わない。

## 戻り値

非負の面積。辺を降順 $a \ge b \ge c$ に並べ、桁落ちを抑えるよう並べ替えたヘロンの公式

$$
S = \frac{1}{4}\sqrt{(a+(b+c))(c-(a-b))(c+(a-b))(a+(b-c))}
$$

で計算する。

## 時間計算量・空間計算量

時間 $O(1)$、追加空間 $O(1)$。

## 注意点

- 非有限値または負の辺長が含まれる場合、および三角不等式を（許容誤差を超えて）満たさない場合は `std::invalid_argument` を送出する。
- 退化三角形の面積は $0$ になる。
