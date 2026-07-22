---
title: 2D Convex Polygon Query (二次元凸多角形クエリ)
documentation_of: ../src/algorithm/geometry/2d/query/convex_polygon_query.hpp
---

同じ凸多角形に対する包含判定を前処理する。

## API

- `ConvexPolygonQuery(polygon)`: 時計回りを反転し、連続重複・中間共線点を除去して保持する。
- `size()`: 正規化後の頂点数を返す。
- `vertices()`: 正規化後の頂点列への `const&` を返す。
- `contains(point)`: 外部0、境界1、内部2を返す。

## API別の時間計算量・空間計算量

- 構築: 時間・保持領域 $O(N)$。
- `size`, `vertices`: 時間・追加領域 $O(1)$。
- `contains`: 時間 $O(\log N)$、追加領域 $O(1)$。

## 注意点

入力は凸多角形でなければならない。`vertices()` の参照はquery objectの生存中のみ有効である。
構築時のturnは角度基準、`contains` の点対辺判定は辺長で正規化した有向距離基準である。許容誤差内で辺上なら境界とし、辺の延長上だけにある点は境界にしない。
