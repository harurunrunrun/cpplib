---
title: Exact Integer Geometry Predicates (厳密整数幾何判定)
documentation_of: ../src/algorithm/integer_geometry/predicates.hpp
---

## API

- `dot(u, v)`, `cross(u, v)`: ベクトルの内積・外積を `__int128_t` で返す。
- `cross(o, a, b)`: $\overrightarrow{oa}\times\overrightarrow{ob}$ を返す。
- `orientation(a, b, c)`, `ccw(a, b, c)`: 左折なら $1$、一直線なら $0$、右折なら $-1$。
- `on_segment(segment, point)`: 閉線分上に点があるかを返す。
- `parallel(a, b)`, `orthogonal(a, b)`: 二線分の方向ベクトルが平行・直交かを返す。
- `segment_intersection_kind(a, b)`: 非交差、端点接触、内点での真の交差、正の長さの重なりを分類する。
- `segments_intersect(a, b)`: 交点が一つ以上あるかを返す。

## API別の時間計算量・空間計算量

各APIは時間 $O(1)$、追加領域 $O(1)$。

## 注意点

判定に許容誤差はない。`orientation`, `ccw`, `on_segment`, `parallel`, `orthogonal`, 線分交差APIは、積を符号と符号なし128 bit絶対値に分けて比較する。このため外積・内積の数値自体が符号付き128 bitに収まらない場合も含め、`int64_t` 座標の全域で正確に判定できる。

`dot` と `cross` は数値を符号付き `__int128_t` で返すため、その途中または結果が表現できなければ `std::overflow_error` を送出する。長さ $0$ の線分も扱い、同一点同士は `touch`、異なる点線分同士は `none` になる。退化線分の方向ベクトルは零ベクトルなので、`parallel` と `orthogonal` はともに真になり得る。
