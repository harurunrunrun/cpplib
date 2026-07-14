---
title: Ear Clipping Triangulation (耳切り法三角形分割)
documentation_of: ../src/algorithm/geometry/2d/ear_clipping_triangulation.hpp
---

単純多角形を耳切り法で三角形分割する。

## API

~~~cpp
using EarClippingTriangle = std::array<std::size_t, 3>;

std::vector<EarClippingTriangle> ear_clipping_triangulation(
    const std::vector<Point>& polygon
);
~~~

polygon の頂点を境界順に渡す。時計回り・反時計回りのどちらも受理する。
戻り値の各要素は入力配列に対する3頂点の添字であり、各三角形は反時計回りになる。
連続する同一点、末尾に重ねて書いた始点、辺上の不要な中間頂点は正規化してから分割する。

## API別の時間計算量・空間計算量

$N$ を入力頂点数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| ear_clipping_triangulation(polygon) | $O(N^2)$ | $O(N)$ |

単純性検査、耳候補の検査、三角形分割を合わせて $O(N^2)$ である。

## 注意点

面積が正の単純多角形を対象とする。自己交差、非連続位置での同一点の再出現、
または正規化後に3頂点未満となる入力には std::invalid_argument を送出する。
座標と中間演算は有限な long double の範囲に収まる必要がある。
