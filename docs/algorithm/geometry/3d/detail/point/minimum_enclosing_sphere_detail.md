---
title: Minimum Enclosing Sphere Internal Detail (最小包含球内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point/minimum_enclosing_sphere_detail.hpp
---

固定点数版と `std::vector` 入力版の公開個別ヘッダが共有する、候補球の構築と包含判定を実装する。

## API

- `minimum_enclosing_sphere_detail::contains(sphere, point)`: 局所スケールに対する相対誤差を用いる内部包含判定。
- `sphere_through_two`, `sphere_through_three`, `sphere_through_four`: 指定した支持点を通る有限な候補球を返す内部関数。構成できない場合は `std::nullopt` を返す。
- `support_sphere(points, size)`: $1 \le \mathrm{size} \le 4$ 点について、1 点から 4 点までの全支持部分集合を列挙し、全点を含む候補のうち半径最小のものを返す内部関数。
- `validate_points(points, size)` などその他の名前空間内の要素: 入力検査、正規化差分、座標復元のための内部関数。

## API別の時間計算量・空間計算量

`contains` と各候補球の構築は $O(1)$。`size` は 4 以下なので、`support_sphere` も時間・追加領域ともに $O(1)$。

## 注意点

内部実装用のヘッダであり、名前・引数・返り値の互換性は公開 API として保証しない。公開個別ヘッダまたは互換用の集約ヘッダを include すること。
`validate_points` は非有限な点に `std::invalid_argument` を、`support_sphere` は有限な候補球を 1 つも構成できない場合に `std::overflow_error` を送出する。
