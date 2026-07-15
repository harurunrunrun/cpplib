---
title: Great-Circle Intersections (大円どうしの交点)
documentation_of: ../src/algorithm/geometry/3d/great_circle_intersections.hpp
---

## API

- `great_circle_intersections(first_normal, second_normal)`: 2大円をそれぞれ含む原点通過平面の法線を受け取り、単位球上の反対な2交点を返す。同じ大円なら`std::nullopt`を返す。

## API別の時間計算量・空間計算量

時間$O(1)$、追加領域$O(1)$。

## 注意点

法線は有限な非零ベクトルでなければならない。平行・反平行な法線は同じ大円を表す。非常に近い大円は丸め誤差に応じて同一と判定する。
