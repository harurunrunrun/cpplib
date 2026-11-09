---
title: Circle Arc Length (円弧長)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/circle/circle_arc_length.hpp
---

円の、指定した中心角に対する弧の長さを求める。

## API

```cpp
long double circle_arc_length(const Circle& circle, long double central_angle);
```

## 引数

`circle` は半径が非負の円、`central_angle` はラジアン単位の有限で非負の角である。

## 戻り値

半径を $r$、中心角を $\theta$ として $r\theta$ を `long double` で返す。

## 時間計算量

時間計算量・追加領域ともに $O(1)$。

## 注意点

円の中心座標・半径が有限でない場合、半径が負の場合、角が負または有限でない場合は `std::invalid_argument` を送出する。
