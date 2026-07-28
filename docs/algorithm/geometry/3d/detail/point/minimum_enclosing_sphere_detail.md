---
title: Minimum Enclosing Sphere Internal Detail (最小包含球内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point/minimum_enclosing_sphere_detail.hpp
---

固定点数とvector入力の公開leafが共有する候補球構築・包含判定を保持する。

## API

- `minimum_enclosing_sphere_detail::contains(sphere, point)`: 局所scaleの
  相対誤差を用いる内部包含判定。
- `sphere_through_two/three/four`: support点を通る有限候補球を返す内部関数。
- `support_sphere(points, size)`: $1\le size\le4$ の全support部分集合を列挙し、
  入力を含む最小候補を返す内部関数。
- その他のnamespace内要素: 入力検査、正規化差分、座標復元用の内部関数。

## API別の時間計算量・空間計算量

`contains` と各候補球構築は $O(1)$。sizeが4以下なので
`support_sphere` も時間・追加領域 $O(1)$。

## 注意点

内部実装用headerであり、名前・引数・返り値の互換性は公開APIではない。
公開leafまたは互換aggregatorをincludeする。
