---
title: Ear Clipping Triangle Type (耳切り法三角形の型)
documentation_of: ../../../../src/algorithm/geometry/2d/ear_clipping_triangle.hpp
---

三角形分割の1三角形を元入力の3頂点添字で保持する型。

## API

```cpp
using EarClippingTriangle = std::array<std::size_t, 3>;
```

公開三角形分割APIが返す要素型である。3添字は反時計回りに並ぶ。

## 時間計算量

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 構築、コピー、添字参照、全列挙 | $O(1)$ | $O(1)$ |

## 注意点

添字は正規化後の並びではなく元の入力頂点列を参照する。
型自体は添字範囲や三角形の非退化性を検査しない。
