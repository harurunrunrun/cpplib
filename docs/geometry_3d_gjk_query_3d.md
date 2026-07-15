---
title: GJK Query 3D (3次元GJK問い合わせ)
documentation_of: ../src/algorithm/geometry/3d/gjk_query_3d.hpp
---

## API

- `gjk_query_3d(first, second, tolerance=1e-12L, max_iterations=96)`: support mapping と縮約単体を用い、交差、距離、両形状上の最近点をまとめて返す。

## API別の時間計算量・空間計算量

- 頂点数を $V_1,V_2$、実反復数を $I$ として時間 $O(I(V_1+V_2))$、追加領域 $O(1)$（返却単体も高々4点）。

## 注意点

有限頂点を持つ空でない凸形状が前提。非正または非有限の許容誤差、反復上限0は `std::invalid_argument`。反復上限到達時は `converged=false`。

両入力を共通平行移動した後、全頂点の広がりで無次元化して core を実行する。`tolerance` はこの正規化座標に対する相対許容誤差であり、一様拡大縮小と共通平行移動に依存しない。同符号の巨大座標同士は除算前に差を取るため、有効な低位bitを保持する。

距離、最近点、終了単体は入力と同じ座標系へ戻す。真の距離・Minkowski差・最近点が `long double` で表現できない場合は `std::overflow_error`。非交差時の `normal` は有限な単位ベクトルである。
