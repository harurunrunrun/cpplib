---
title: Normalized Convex Polygon (正規化凸多角形)
documentation_of: ../../../../../../src/algorithm/geometry/2d/detail/point_set_point/convex_calipers_detail.hpp
---

rotating calipers 系のクエリで共有する、正規化済みの凸多角形。

```cpp
#include "../../../../../../src/algorithm/geometry/2d/detail/point_set_point/convex_calipers_detail.hpp"

NormalizedConvexPolygon polygon(vertices);
```

## 構築

```cpp
explicit NormalizedConvexPolygon(std::vector<Point> polygon);
```

周回順に並んだ弱凸多角形を受け取り、`normalize_convex_polygon` を用いて次の形に正規化する。

- 向きを反時計回りにする。
- 連続する重複頂点、末尾に置かれた先頭頂点の複製、辺上の中間頂点を除く。
- 辞書順最小の頂点を `vertices()[0]` にする。
- 全点が同一直線上なら両端の 2 点に、全点が同一点なら 1 点にする。

入力は時計回り・反時計回りのどちらでもよく、すでに正規化された入力もそのまま使える。
左折と右折が混在する非凸入力には `std::invalid_argument` を送出する。

各アルゴリズムが返す添字は、入力列ではなく `vertices()` の添字である。

## 参照

```cpp
std::size_t size() const;
bool empty() const;
const std::vector<Point>& vertices() const;
const std::vector<long double>& edge_angles() const;
```

`edge_angles()` は、頂点数が 3 以上のときの各辺 `vertices()[i]`→`vertices()[i + 1]` の偏角を、狭義単調増加になるよう $2\pi$ ずつ補正した列である（頂点数 2 以下では空）。方向に対する支持点クエリの二分探索に使う。

`CONVEX_POLYGON_NPOS`（`std::size_t` の最大値）は、空集合に対して頂点を返せない API の番兵値である。

名前空間 `rotating_calipers_detail` には、各アルゴリズムが共有する補助関数（`scalar_compare`、`squared_distance`、`doubled_triangle_area`、`cyclic_vertex`、`oriented_side`、`contains_normalized_convex_polygon`）も置かれている。`contains_normalized_convex_polygon` は外部なら 0、境界上なら 1、内部なら 2 を $O(\log N)$ で返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(N)$ |
| `size`, `empty`, `vertices`, `edge_angles` | $O(1)$ | $O(1)$ |

判定誤差は `advanced_convex_geometry.hpp` と同じく、`GEOMETRY_EPS` とスケールを考慮した述語に従う。

## 注意点

座標と中間演算は有限な `long double` の範囲に収まる必要がある。境界・退化の判定には、各 API に明記した許容誤差を用いる。
