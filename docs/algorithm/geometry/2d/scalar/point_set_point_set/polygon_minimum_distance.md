---
title: Minimum Distance between General Polygons (一般多角形間の最小距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set_point_set/polygon_minimum_distance.hpp
---

凸性を仮定しない 2 つの単純多角形について、閉領域間の最小ユークリッド距離を返す。
領域が交差する場合や一方が他方を包含する場合は $0$ を返す。境界どうしの距離は、各辺の AABB（軸平行包含箱）の階層を作り、下界の小さい節点対から順に探索して求める。

## API

```cpp
long double polygon_minimum_distance(
    const std::vector<Point>& first,
    const std::vector<Point>& second
);
```

## 時間計算量

頂点数を $N, M$ とする。

- AABB 階層の構築: $O(N \log N + M \log M)$
- 包含判定: $O(N + M)$
- 距離探索: 通常は箱の下界による枝刈りが効く。最悪時間計算量は $O(NM)$
- 追加空間計算量: $O(N + M)$

## 注意点

- 各入力は、頂点を境界順に並べた有限座標の単純多角形とする。頂点数 $1$ は点、頂点数 $2$ は線分として扱う。
- 空の入力、または非有限な座標を含む入力には `std::invalid_argument` を送出する。
- 包含の判定は、頂点数 $3$ 以上の側について相手の先頭頂点が内部または境界上にあるかで行う。自己交差多角形の内部は対象外である。
