---
title: Polygon Boundary Point Query (多角形境界点クエリ)
documentation_of: ../src/algorithm/geometry/2d/polygon_boundary_points/polygon_boundary_point_query.hpp
---

同じ多角形に対する境界点列挙を前処理する。

## API

- `PolygonBoundaryPointQuery(polygon)`: 凸高速経路を検査・構築し、利用できなければ一般経路を保持する。
- `uses_convex_fast_path()`: 凸高速経路が利用可能か返す。
- `polygon()`: 元の頂点列への `const&` を返す。
- `enumerate(points, unique = true)`: 辺上にある入力点のindexを辺順・辺上の進行順で返す。

## API別の時間計算量・空間計算量

- 構築: 辺数を $N$ として時間 $O(N\log N)$、保持領域 $O(N)$。
- metadata参照: 時間・追加領域 $O(1)$。
- 凸高速経路: 点数 $M$、出力長 $K$ として時間 $O(M\log N+K\log(K+1))$、追加領域 $O(N+M+K)$。
- 一般経路: 時間 $O(NM\log M+K)$、追加領域 $O(M+K)$。

## 注意点

高速経路が不適用または数値的に曖昧なら自動的に一般経路へ戻る。`polygon()` の参照はquery objectの生存中のみ有効である。
