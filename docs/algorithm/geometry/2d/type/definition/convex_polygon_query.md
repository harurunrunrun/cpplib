---
title: 2D Convex Polygon Query (二次元凸多角形クエリ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/convex_polygon_query.hpp
---

同じ凸多角形に対する包含判定のために前処理を行う。

## API

- `explicit ConvexPolygonQuery(std::vector<Point> polygon)`: 連続する重複点と末尾に重ねた始点を除き、時計回りなら反転し、共線な中間頂点を除去して保持する。全頂点が一直線上にある場合は両端の 2 点（全点が一致するなら 1 点）に退化させる。
- `int size() const`: 正規化後の頂点数を返す。
- `const std::vector<Point>& vertices() const`: 正規化後の頂点列への参照を返す。
- `int contains(const Point& point) const`: 外部なら $0$、境界上なら $1$、内部なら $2$ を返す。頂点数 $0$ では常に $0$、頂点数 $1$ ・ $2$ ではそれぞれ点・線分上にあるとき $1$ を返す。

## 計算量

正規化後の頂点数を $N$ とする。

- 構築: 時間・保持領域 $O(N)$。
- `size`, `vertices`: 時間・追加空間 $O(1)$。
- `contains`: 時間 $O(\log N)$、追加空間 $O(1)$。

## 注意点

- 入力は凸多角形でなければならない。
- `vertices()` が返す参照は、このオブジェクトが生存している間だけ有効である。
- 構築時の折れ曲がりの判定は角度を基準に、`contains` における点と辺の位置判定は辺の長さで正規化した有向距離を基準に行う。許容誤差内で辺上にある点は境界とし、辺の延長線上にあるだけの点は境界としない。
