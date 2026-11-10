---
title: Circle3 Finite Check (三次元円の有限値判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/circle/circle3_is_finite.hpp
---

## API

- `geometry3d_is_finite(circle)`: `Circle3` の中心・法線・半径がすべて有限なら `true` を返す（`noexcept`）。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

法線が非零か、半径が非負かは検査しない。契約の検査には `geometry3d_validate` を使う。
