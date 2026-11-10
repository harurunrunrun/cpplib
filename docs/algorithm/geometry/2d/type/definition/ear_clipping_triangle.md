---
title: Ear Clipping Triangle Type (耳切り法三角形の型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/ear_clipping_triangle.hpp
---

三角形分割の 1 つの三角形を、元の入力の 3 頂点の添字で保持する型。

## API

```cpp
using EarClippingTriangle = std::array<std::size_t, 3>;
```

公開されている三角形分割 API が返す要素の型である。3 つの添字は反時計回りに並ぶ。

## 時間計算量

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 構築、コピー、添字アクセス、全列挙 | $O(1)$ | $O(1)$ |

## 注意点

添字は正規化後の並びではなく、元の入力頂点列を参照する。
型自体は添字の範囲や三角形の非退化性を検査しない。
