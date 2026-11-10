---
title: Circles through Two Points (2 点を通る指定半径の円)
documentation_of: ../../../../../../src/algorithm/geometry/2d/circle/point_point/circles_through_two_points.hpp
---

## API

```cpp
std::vector<Circle> circles_through_two_points(
    const Point& first,
    const Point& second,
    long double radius
);
```

2 点を円周上に持つ、半径 `radius` の円を列挙する。

## 引数

`first`, `second` は円が通る 2 点、`radius` は非負の半径である。

## 戻り値

0〜2 個の `Circle` を中心の辞書順で返す。

- 2 点間の距離が直径 $2r$ を超える場合は空。
- 2 点間の距離が直径と（許容誤差の範囲で）等しい場合は、中点を中心とする 1 個。
- それ以外の相異なる 2 点では 2 個。
- 2 点が完全に一致し、`radius` が 0 の場合は、その点を中心とする半径 0 の円 1 個。

## API別の時間計算量・空間計算量

時間・出力・追加領域とも $O(1)$。

## 注意点

- 2 点が一致し、半径が正の場合は解が無限に存在するため `std::domain_error` を送出する。
- 半径が負または非有限の場合は `std::invalid_argument` を送出する。比較の中間値が非有限になった場合も `std::invalid_argument` を送出する。
- 接するかどうかの判定（距離と直径の比較、高さの 2 乗の符号）は、比較する各項の大きさに `GEOMETRY_EPS` を掛けた相対許容誤差で行う。中間値は有限な `long double` に収まる必要がある。
- 2 点が一致するかは弦の長さがちょうど 0 かどうかで判定し、非常に短い正の弦を同一点とはみなさない。
