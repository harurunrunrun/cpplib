---
title: Convex Layers (凸層) [convex_layers]
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer_set/mixed/convex_layers.hpp
---

点集合の凸包の境界上にある点をすべて取り除く操作を繰り返し、各入力点が何回目に取り除かれるかを求める。onion decomposition とも呼ばれる。

## `convex_layers`

```cpp
template<std::integral Coordinate>
std::vector<int> convex_layers(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
);
```

`answer[i]` は `points[i]` が取り除かれる層の番号で、最も外側の層を 1 とする。
空の入力には空列を返す。凸包の辺上にある共線な点も、その辺の両端と同じ層ですべて取り除く。同じ座標が複数回現れた場合は、すべて同じ層になる。

座標を辞書順に整列して重複をまとめ、上側凸包と下側凸包をそれぞれ 1 本の削除専用（decremental）凸包として持つ。
各節点の凸包チェーンをつなぎ替えながら、接線のリンクを固定の木の下方向へ移していく。
各リンクが木の各高さを通過する回数は償却定数回なので、全削除を通した凸包の更新時間は $O(U \log U)$ である。

## API別の時間計算量・空間計算量

$N$ を入力点数、$U$ を相異なる座標の数とする。

| API・操作 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `convex_layers(points)` | $O(N \log N)$ | $O(N)$ |
| 戻り値の 1 要素の参照 | $O(1)$ | $O(1)$ |

内訳は、整列が $O(N \log N)$、重複の集約・層の列挙・元の添字への復元が $O(N)$、2 本の削除専用凸包に対する全更新が $O(U \log U)$ である。

## 注意点

`Coordinate` は整数型でなければならない。座標を符号付き 128 bit 整数に変換した値について、座標差の積と外積が `__int128_t` の範囲に収まることを前提とする（たとえば座標の絶対値が $2^{62}$ 未満なら十分）。
浮動小数点の許容誤差は使わず、境界・共線の判定は整数の外積で厳密に行う。
