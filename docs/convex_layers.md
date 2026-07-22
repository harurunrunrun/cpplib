---
title: Convex Layers (凸層) [convex_layers]
documentation_of: ../src/algorithm/geometry/2d/point_collection/convex_layers.hpp
---

点集合の凸包境界上にある点を全て取り除く操作を繰り返し、
各入力点が何回目に取り除かれるかを求める。onion decompositionとも呼ばれる。

## `convex_layers`

```cpp
template<std::integral Coordinate>
std::vector<int> convex_layers(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
);
```

`answer[i]` は `points[i]` が取り除かれる層で、最外層を1とする。
空入力には空列を返す。同一直線上の凸包辺にある点も、その辺の両端と
同じ層で全て取り除く。同一座標が複数回現れた場合は全て同じ層になる。

座標を辞書順に整列して重複を集約し、上側と下側に減少型凸包を1本ずつ持つ。
各節点の凸包chainをspliceしながら接線linkを固定木の下方向へ移す。
各linkが各木の高さで通過する回数は償却定数回なので、全削除を通した
凸包更新時間は $O(U\log U)$ である。

## API別の時間計算量・空間計算量

$N$ を入力点数、$U$ を異なる座標数とする。

| API・操作 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `convex_layers(points)` | $O(N\log N)$ | $O(N)$ |
| 戻り値の1要素参照 | $O(1)$ | $O(1)$ |

整列が $O(N\log N)$、重複集約・層の列挙・元添字への復元が $O(N)$、
2本の減少型凸包に対する全更新が $O(U\log U)$ である。

## 注意点

`Coordinate` は整数型でなければならない。座標を符号付き128-bit整数へ
変換した値、および座標差の積と外積が `__int128_t` の範囲に収まることを
前提とする。浮動小数点の許容誤差は使用せず、境界・共線判定は整数外積で行う。
