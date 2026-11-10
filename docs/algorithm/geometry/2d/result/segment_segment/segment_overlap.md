---
title: Collinear Segment Overlap (共線線分の共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/segment_segment/segment_overlap.hpp
---

2 本の線分が共線であるとき、その共通部分を線分として求める。

## API

```cpp
std::optional<Segment> segment_overlap(
    const Segment& first,
    const Segment& second
);
```

## 引数

2 本の線分を渡す。

## 戻り値

共線な共通部分を、端点を辞書順に並べた線分として返す。1 点で接する場合は退化した線分（両端点が等しい線分）を返す。共通部分がない場合や共線でない場合は `std::nullopt` を返す。

## 時間計算量

時間計算量 $O(1)$、追加領域 $O(1)$。

## 注意点

浮動小数点の比較には `GEOMETRY_EPS` を用いる。退化した線分は 1 点として扱い、非退化な線分どうしの共線性は、一方の線分の支持直線に対するもう一方の端点の位置で判定する。
