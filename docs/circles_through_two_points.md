---
title: Circles through Two Points (2点を通る指定半径円)
documentation_of: ../src/algorithm/geometry/2d/shape/circles_through_two_points.hpp
---

## API

- `circles_through_two_points(first, second, radius)`: 2点を円周上に持つ指定半径円を列挙する。

## 引数

`first`, `second` は通過点、`radius` は非負半径である。

## 戻り値

中心の辞書順で0〜2個の `Circle` を返す。

## API別の時間計算量・空間計算量

時間・出力・追加領域 $O(1)$。

## 注意点

異なる2点間距離が直径を超える場合は空。同一点と正半径では解が無限にあるため `std::domain_error` を送出する。負半径には `std::invalid_argument` を送出する。

- 円の中心座標と半径は有限値であること。非有限値または負半径にはstd::invalid_argumentを送出する。
- 接触・包含の距離二乗や4次量は、比較する各項の大きさにGEOMETRY_EPSを掛けた相対許容誤差で判定する。中間値は有限なlong doubleに収まること。
- 2点が厳密に同一かはchord長0で判定し、tinyな正のchordを同一点へ丸めない。
