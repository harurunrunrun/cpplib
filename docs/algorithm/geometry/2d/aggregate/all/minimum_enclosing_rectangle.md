---
title: Minimum Enclosing Rectangle Aggregator (最小外接長方形集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/minimum_enclosing_rectangle.hpp
---

凸集合の最小外接長方形に関する型と2種類の入力APIをまとめて読み込む
後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `minimum_enclosing_rectangle_result.hpp` | `MinimumEnclosingRectangleResult` |
| `minimum_enclosing_rectangle_normalized.hpp` | 正規化済み凸多角形を受け取るoverload |
| `minimum_enclosing_rectangle_points.hpp` | `vector<Point>` を受け取るoverload |

依存を最小にしたい場合は必要なleaf headerだけを直接includeする。従来の
`minimum_enclosing_rectangle.hpp` は引き続き全APIを提供する。

## 集約されるAPI


- `minimum_enclosing_rectangle(polygon)`: 凸集合を含む面積最小の長方形を返す。
- `MinimumEnclosingRectangleResult`: `corners`, `width`, `height`, `area`, `valid` を保持する。

## 引数

`polygon` は `NormalizedConvexPolygon` または弱凸な周回列である。

## 戻り値

`corners` は反時計回りの4頂点。空集合では `valid=false`、それ以外は `true`。

## API別の時間計算量・空間計算量

- 正規化済みoverload: 時間 $O(N)$、追加領域 $O(1)$。
- 頂点列overload: 時間・領域 $O(N)$。
- 結果field参照: 時間・領域 $O(1)$。

## 注意点

点・線分では面積と高さが0になる。複数解がある場合はいずれか1つを返す。
