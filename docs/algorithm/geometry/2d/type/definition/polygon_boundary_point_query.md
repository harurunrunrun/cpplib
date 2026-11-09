---
title: Polygon Boundary Point Query (多角形境界点クエリ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/polygon_boundary_point_query.hpp
---

同じ多角形に対して、境界上にある点を列挙するための前処理を行う。

## API

- `explicit PolygonBoundaryPointQuery(std::vector<Point> polygon)`: 凸多角形向けの高速経路を検査・構築し、利用できなければ一般経路を使うよう設定する。
- `bool uses_convex_fast_path() const`: 凸多角形向けの高速経路が使われるかを返す。
- `const std::vector<Point>& polygon() const`: 元の頂点列への参照を返す。
- `std::vector<int> enumerate(const std::vector<Point>& points, bool unique = true) const`: 辺上にある入力点の添字を、辺の順、同じ辺の上では辺に沿った進行順に返す。`unique == true` なら各点を最初に該当した 1 辺についてだけ返し、`false` なら点が載るすべての辺について返す（頂点上の点は複数回現れ得る）。

## 計算量

- 構築: 辺数を $N$ として時間 $O(N \log N)$、保持領域 $O(N)$。
- `uses_convex_fast_path`, `polygon`: 時間・追加空間 $O(1)$。
- 高速経路の `enumerate`: 点数を $M$、出力長を $K$ として時間 $O(M \log N + K \log (K+1))$、追加空間 $O(N + M + K)$。
- 一般経路の `enumerate`: 時間 $O(NM + K \log (K+1))$、追加空間 $O(M + K)$。
  各辺と各点の包含判定が $O(NM)$、辺ごとの進行順のソートの合計が $O(K \log (K+1))$ である。

## 注意点

- 構築時に高速経路が適用できない、または数値的に曖昧と判定された場合は、自動的に一般経路を使う。
- 凸多角形上の辺判定では、正規化した有向距離と真の線分上判定を併用し、細長い多角形上の点を角度の許容誤差だけで境界外と誤判定しないようにしている。
- 辺上のパラメータの探索許容誤差は、辺長を $L$ として概ね $\varepsilon / L$（$\varepsilon$ は `GEOMETRY_EPS`）と機械の丸め誤差の和であり、短い辺でも一般経路と同じ端点の許容誤差を保つ。
- `polygon()` が返す参照は、このオブジェクトが生存している間だけ有効である。
