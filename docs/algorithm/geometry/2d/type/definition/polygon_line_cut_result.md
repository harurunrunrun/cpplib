---
title: Polygon Line Cut Result (多角形直線切断の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/polygon_line_cut_result.hpp
---

一般多角形を有向直線で切断した両閉半平面側の連結成分を保持する結果型。

## API

```cpp
struct PolygonLineCutResult {
    std::vector<std::vector<Point>> left;
    std::vector<std::vector<Point>> right;
};
```

- `left`: 有向直線の左閉半平面と多角形の共通部分を、反時計回りの連結成分列で保持する。
- `right`: 有向直線の右閉半平面と多角形の共通部分を、反時計回りの連結成分列で保持する。

既定構築では両fieldとも空である。

## 時間計算量

左側の全頂点数を $L$、右側の全頂点数を $R$ とする。

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 既定構築、各fieldの参照、`size()` | $O(1)$ | $O(1)$ |
| `left` の全列挙 | $O(L)$ | $O(1)$ |
| `right` の全列挙 | $O(R)$ | $O(1)$ |

## 注意点

各成分は少なくとも3頂点を持つ非退化な単純多角形である。
切断直線上の境界は左右の両方に含まれる。
