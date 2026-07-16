---
title: Visibility Graph (可視グラフ)
documentation_of: ../src/algorithm/geometry/2d/visibility_graph.hpp
---

単純多角形の頂点同士を、多角形の内部または境界だけを通る直接の線分で結ぶ可視グラフを構築する。

## Edge

~~~cpp
struct VisibilityGraphEdge {
    size_t first;
    size_t second;
    long double distance;
};
~~~

firstはsecondより小さく、distanceは2頂点間のEuclid距離。

## API

~~~cpp
vector<VisibilityGraphEdge> visibility_graph(
    const vector<Point>& polygon
)
~~~

多角形の隣接頂点は常に辺として含める。非隣接頂点は、開線分が境界を横切らず、中点が多角形の内部または境界にある場合に含める。戻り値はfirst, secondの辞書順。

## 時間計算量

頂点数をNとするとO(N^3)。各頂点pairについて全頂点と全辺を検査する。

## 空間計算量

戻り値を含めてO(N^2)、作業領域はO(1)。

## 注意点

- polygonは3頂点以上の有限座標からなる、面積が非零の単純多角形であること。退化辺・自己交差があればinvalid_argument。
- 入力順は時計回り・反時計回りのどちらでもよい。
- 第3のpolygon頂点を開線分上に通るpairは直接辺として省く。その頂点を経由する辺へ分割できるため、可視経路の到達可能性は失わない。
- 境界に沿う線分は含め、外部へ出る線分と境界を横切る線分は含めない。
- 幾何判定にはGEOMETRY_EPSを使うため、許容誤差以下の退化配置を避ける。
