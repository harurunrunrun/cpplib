---
title: Normalized Convex Polygon (正規化凸多角形)
documentation_of: ../src/algorithm/geometry/convex_calipers_common.hpp
---

rotating calipers 系の問い合わせで共有する正規化済み凸多角形。

```cpp
#include "src/algorithm/geometry/convex_calipers_common.hpp"

NormalizedConvexPolygon polygon(vertices);
```

## 構築

```cpp
explicit NormalizedConvexPolygon(std::vector<Point> polygon);
```

周回順の弱凸多角形を受け取り、次の形に正規化する。

- 向きを反時計回りにする。
- 連続重複、末尾に置かれた先頭頂点、辺上の中間頂点を除く。
- 辞書順最小の頂点を `vertices()[0]` にする。
- 全点が同一直線なら両端2点、全点が同一点なら1点にする。

時計回り・反時計回りのどちらでもよい。すでに正規化された入力もそのまま使える。
左右の折れ曲がりが混在する非凸入力には `std::invalid_argument` を送出する。

各アルゴリズムが返す添字は、入力列ではなく `vertices()` の添字である。

## 参照

```cpp
std::size_t size() const;
bool empty() const;
const std::vector<Point>& vertices() const;
```

`CONVEX_POLYGON_NPOS` は、空集合に対して頂点を返せない API の番兵値である。

## 計算量

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| constructor | $O(N)$ | $O(N)$ |
| `size`, `empty`, `vertices` | $O(1)$ | $O(1)$ |

辺方向の角度列も構築時に保持し、方向 support 問い合わせの二分探索に使う。

判定誤差は `advanced_convex_geometry.hpp` と同じ `GEOMETRY_EPS` および
scale-aware predicate に従う。
