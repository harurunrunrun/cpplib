---
title: Geometry3DExpansion Type (3次元幾何用浮動小数点展開型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/geometry3d_expansion.hpp
---

浮動小数点展開（複数の浮動小数点数の和で 1 つの値を厳密に表す表現）を構成する成分列の公開型を提供する。

## API

- `Geometry3DExpansion`: 絶対値の小さい成分から順に並べる `std::vector<long double>` の別名。

## API別の時間計算量・空間計算量

型別名の利用自体は時間・追加領域ともに $O(1)$。要素操作の計算量は `std::vector` に従う。

## 注意点

- 算術演算を使う場合は、各演算のヘッダ、または互換用の集約ヘッダ `expansion_arithmetic.hpp` をインクルードする。
