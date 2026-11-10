---
title: Polygon Line Cut Result (多角形直線切断の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/polygon_line_cut_result.hpp
---

一般の多角形を有向直線で切断したとき、左右それぞれの閉半平面側の連結成分を保持する結果型。

## API

```cpp
struct PolygonLineCutResult {
    std::vector<std::vector<Point>> left;
    std::vector<std::vector<Point>> right;
};
```

- `left`: 有向直線の左側の閉半平面と多角形の共通部分を、反時計回りの連結成分の列として保持する。
- `right`: 有向直線の右側の閉半平面と多角形の共通部分を、反時計回りの連結成分の列として保持する。

デフォルト構築では両フィールドとも空である。

## 時間計算量

左側の全頂点数を $L$、右側の全頂点数を $R$ とする。

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| デフォルト構築、各フィールドの参照と `size()` | $O(1)$ | $O(1)$ |
| `left` の全列挙 | $O(L)$ | $O(1)$ |
| `right` の全列挙 | $O(R)$ | $O(1)$ |

## 注意点

各成分は 3 頂点以上をもつ非退化な単純多角形である。
切断直線上の境界は左右の両方に含まれる。
