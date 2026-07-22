---
title: Minimum Enclosing Rectangle (最小外接長方形)
documentation_of: ../src/algorithm/geometry/2d/shape/minimum_enclosing_rectangle.hpp
---

## API

- `minimum_enclosing_rectangle(polygon)`: 凸集合を含む面積最小の長方形を返す。
- `MinimumEnclosingRectangleResult`: `corners`, `width`, `height`, `area`, `valid` を保持する。

## 引数

`polygon` は `NormalizedConvexPolygon` または弱凸な周回列である。

## 戻り値

`corners` は反時計回りの4頂点。空集合では `valid=false`、それ以外は `true`。

## API別の時間計算量・空間計算量

- 正規化済みoverload: 時間 $O(N)$、追加領域 $O(1)$。
- 頂点列overload: 時間・領域 $O(N)$。
- 結果field参照: 時間・領域 $O(1)$。

## 注意点

点・線分では面積と高さが0になる。複数解がある場合はいずれか1つを返す。
