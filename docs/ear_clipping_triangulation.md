---
title: Ear Clipping Triangulation (耳切り法三角形分割)
documentation_of: ../src/algorithm/geometry/2d/shape/ear_clipping_triangulation.hpp
---

単純多角形を三角形分割する。既存のAPI名は互換性のため維持しているが、
実装は二乗時間の逐次耳切りではない。sweepで $y$-単調多角形へ分割し、
各単調面をstackで線形時間に三角形分割する。

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

入力が時計回りなら内部で反転するが、戻り値の添字は常に元の
`polygon` を参照する。

## API別の時間計算量・空間計算量

$N$ を入力頂点数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `ear_clipping_triangulation(polygon)` | $O(N\log N)$ | $O(N)$ |

Bentley--Ottmann法による単純性検査、単調分割sweep、分割面の
平面埋込み復元がそれぞれ $O(N\log N)$。共線点除去と全単調面の
三角形分割は合計 $O(N)$。生成される三角形数も $O(N)$ である。

## 注意点

絶対面積が正（符号付き面積が非零）の単純多角形を対象とする。自己交差、非連続位置での同一点の再出現、
隣接辺の重なり、または正規化後に3頂点未満となる入力には
`std::invalid_argument` を送出する。
座標と中間演算は有限な `long double` の範囲に収まる必要がある。
向き判定では先頭頂点を原点に移し、多角形の広がりの二乗をscaleとする符号判定を行う。
したがって、座標平行移動や一様scaleによって生の符号付き面積だけが絶対許容誤差をまたぐ問題を避ける。
