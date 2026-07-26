---
title: Exact Lattice Point Type (厳密格子点型)
documentation_of: ../../../src/integer_geometry/lattice/types.hpp
---

任意精度整数座標の格子点を、有理数座標の一般点とは別型で表す。

## API

- `IntegerPoint{x, y}`: 任意精度整数座標の点を構築する。
- `operator==`, `operator<=>`: 厳密な等値比較と辞書順比較を行う。
- `rational_point(point)`: 格子点を有理数座標の `Point` に変換する。

## API別の時間計算量・空間計算量

座標の最大bit長を $L$ とする。

- 構築と比較: 時間 $O(L)$、座標保存領域 $O(L)$。
- `rational_point`: 時間・追加領域ともに $O(L)$。

## 注意点

整数格子に固有のPickの定理などには `IntegerPoint` を使い、一般の有理数幾何には `Point` を使う。
