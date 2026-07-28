---
title: General Polygon Boolean Operations (一般多角形集合演算)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set_point_set/polygon_boolean.hpp
---

even-odd 規則で表した2つの多角形集合について、和・積・差・対称差の境界閉路を構築する。

## API

- `PolygonBooleanOperation` は `set_union`, `intersection`, `difference`, `symmetric_difference` の4種。
- `polygon_boolean(first, second, operation)` は単純多角形2個、または `vector<vector<Point>>` で外周・穴・複数成分を受け取る。
- 戻り値は結果集合の内部を左に見る向き付き閉路列であり、穴は外周と逆向きになる。

## 時間計算量

入力総辺数を $N$、分割後の境界片数を $K=O(N^2)$ とする。全辺対の交点生成と
各辺上のevent sortは $O(N^2\log N)$。各境界片の左右sampleを入力全ringへ照合する
分類が $O(NK)$、endpoint clustering・重複除去・角度順stitchが $O(K\log K)$。
全体は $O(N^2\log N+NK+K\log K)$ 時間、最悪 $O(N^3)$、
$O(N^2+K)$ 空間。

## 注意点

各入力 ring は自己交差しない非退化単純多角形。ring 間の包含は even-odd 規則で解釈する。
接触・共線重複辺を分割して処理する。有限精度の `long double` 幾何述語を用い、座標は有限値でなければならない。
