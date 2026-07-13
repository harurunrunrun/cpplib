---
title: Convex Layers (凸層)
documentation_of: ../src/algorithm/geometry/convex_layers.hpp
---

点集合の凸包の境界上にある点をすべて取り除く操作を繰り返し、各点が取り除かれる回を求める。
各層の左右の凸包鎖を削除対応の分割統治木で管理する。

## `convex_layers`

```cpp
std::vector<int> layer = convex_layers(points);
```

`points`は`std::vector<std::pair<Coordinate, Coordinate>>`で、`Coordinate`は整数型とする。
返り値の`layer[i]`は点`points[i]`が取り除かれる1始まりの回数である。
凸包の頂点だけでなく辺上の点も同じ回に取り除く。同一座標の入力点は同じ層に属する。

外積と凸包鎖の比較式は128-bit符号付き整数で計算する。中間値がその範囲に収まる必要があり、
Library Checkerの$0\leq x,y\leq10^6$では常に満たされる。

## API別の時間計算量・空間計算量

$N$を入力点数、$U$を異なる座標の数とする。

| API・操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `convex_layers` | $O(N\log N+U\log^2 U)$ | $O(N+U)$ |
| 返り値の1要素の参照 | $O(1)$ | $O(1)$ |

## 注意点

座標と中間演算は有限な `long double` の範囲に収まる必要がある。境界・退化判定には各APIで明記した許容誤差を用いる。
