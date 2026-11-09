---
title: Simple Polygon Triangulation (単純多角形三角形分割)
documentation_of: ../../../../../../src/algorithm/geometry/2d/triangle/point_set/simple_polygon_triangulation.hpp
---

単純多角形を三角形分割するための汎用 API。

## API

```cpp
std::vector<EarClippingTriangle> simple_polygon_triangulation(
    const std::vector<Point>& polygon
);
```

`polygon` の頂点を境界順に渡すと、入力配列の頂点添字からなる三角形の列を返す。
時計回り・反時計回りのどちらも受理し、返す三角形は反時計回りにそろえる。
現在の実装は `ear_clipping_triangulation` をそのまま呼び出し、同じ正規化、単純性の検査、
$y$ 単調分割、スタックによる単調多角形の三角形分割を用いる。

## 計算量

入力頂点数を $N$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `simple_polygon_triangulation(polygon)` | $O(N \log N)$ | $O(N)$ |

この計算量は、単純性の検査、単調分割、平面埋め込みからの面の復元を含む。
すべての単調多角形に対する三角形分割の合計は $O(N)$ である。

## 注意点

面積が正の単純多角形を対象とする。連続する重複頂点、閉路を表す末尾の始点、
辺上の中間頂点は除去される。自己交差、連続しない位置での頂点の重複、
隣接辺の重なり、その他の退化した入力には `std::invalid_argument` を送出する。
座標と中間演算の値は有限な `long double` の範囲に収まる必要がある。
