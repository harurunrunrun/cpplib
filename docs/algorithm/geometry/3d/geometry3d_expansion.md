---
title: Geometry3DExpansion Type (3次元幾何用浮動小数点展開型)
documentation_of: ../../../../src/algorithm/geometry/3d/geometry3d_expansion.hpp
---

浮動小数点展開を構成する成分列の公開型を提供します。

## API

- `Geometry3DExpansion`: 絶対値の小さい成分から並べる `std::vector<long double>` の別名です。

## API別の時間計算量・空間計算量

型別名の利用は時間・追加空間ともに $O(1)$ です。要素操作の計算量は `std::vector` に従います。

## 注意点

算術演算は各演算leafまたは互換aggregator `expansion_arithmetic.hpp` をincludeしてください。