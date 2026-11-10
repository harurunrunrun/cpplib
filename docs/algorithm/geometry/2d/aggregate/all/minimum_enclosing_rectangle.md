---
title: Minimum Enclosing Rectangle Aggregator (最小外接長方形集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/minimum_enclosing_rectangle.hpp
---

凸集合の最小外接長方形に関する型と、2 種類の入力を受け取る API をまとめて読み込む後方互換の集約ヘッダである。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `minimum_enclosing_rectangle_result.hpp` | `MinimumEnclosingRectangleResult` |
| `minimum_enclosing_rectangle_normalized.hpp` | 正規化済み凸多角形を受け取るオーバーロード |
| `minimum_enclosing_rectangle_points.hpp` | `std::vector<Point>` を受け取るオーバーロード |

依存を最小にしたい場合は、必要な leaf header だけを直接 include する。従来どおり `minimum_enclosing_rectangle.hpp` を include すれば全 API を使える。

## 集約される API

```cpp
struct MinimumEnclosingRectangleResult {
    std::array<Point, 4> corners;
    long double width;
    long double height;
    long double area;
    bool valid;
};

MinimumEnclosingRectangleResult minimum_enclosing_rectangle(const NormalizedConvexPolygon& polygon);
MinimumEnclosingRectangleResult minimum_enclosing_rectangle(std::vector<Point> polygon);
```

凸集合を含む面積最小の長方形を返す。`polygon` は `NormalizedConvexPolygon` または弱凸な周回列である。

## 戻り値

`corners` は長方形の 4 頂点を反時計回りに並べたものである。`width` は基準とした辺方向の幅、`height` はそれに垂直な方向の高さ、`area` は面積である。空集合では `valid == false`、それ以外では `valid == true` となる。

## 時間計算量・空間計算量

頂点数を $N$ とする。

- 正規化済みのオーバーロード: 時間 $O(N)$、追加の空間 $O(1)$。
- 頂点列のオーバーロード: 時間・空間ともに $O(N)$。
- 結果のフィールドの参照: 時間・空間ともに $O(1)$。

## 注意点

点・線分に退化した凸集合では、面積と高さが $0$ になる。解が複数ある場合はいずれか 1 つを返す。
