---
title: Polygon3 (三次元多角形)
documentation_of: ../src/algorithm/geometry/3d/core/polygon3.hpp
---

## API

- `Polygon3`: 頂点を境界順に保持する`std::vector<Point3>`の別名。

## API別の時間計算量・空間計算量

- 型宣言は時間・追加領域 $O(1)$。頂点列の操作量は`std::vector`に従う。

## 注意点

各アルゴリズムへ渡す頂点列は同一平面上の単純多角形とする。
