---
title: QuickHull 3D (三次元QuickHull)
documentation_of: ../src/algorithm/geometry/3d/quickhull_3d.hpp
---

## API

- `quickhull_3d(points)`: 初期四面体から各支持面の最遠可視点を選び、horizonを張り替えて3D凸包を構成する。重複点と退化次元にも対応する。

## API別の時間計算量・空間計算量

- 点数を $N$、生成面数を $F$ として平均的に $O(NF)$、この全走査実装の最悪時間 $O(NF^2)$、領域 $O(N+F)$。

## 注意点

判定には適応的 `orient3d` を使う。非有限座標を拒否し、共線・共面時は退化対応の増分凸包へ移る。
