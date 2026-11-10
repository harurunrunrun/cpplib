---
title: Visibility Graph from Polygon Points (多角形頂点列の可視グラフ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/visibility_graph_points.hpp
---

単純多角形の全頂点を節点とする無向の可視グラフを構築する。

## API

```cpp
std::vector<VisibilityGraphEdge> visibility_graph(
    const std::vector<Point>& polygon
);
```

隣接する頂点どうしは常に辺として含める。隣接しない頂点どうしは、それらを結ぶ開線分が多角形の外部を通らず、第 3 の多角形頂点も通らない場合に辺として含める。返り値の各辺は `first < second` を満たす添字 `first`, `second` と、2 頂点間の距離 `distance` を持ち、`(first, second)` の辞書順に並ぶ。各始点からの可視性は、偏角順の回転スイープでまとめて判定する。

## 時間計算量

頂点数を $N$、返す辺の数を $K$ とする。

| API | 時間計算量 | 空間計算量（返り値を含む） |
| --- | --- | --- |
| `visibility_graph(polygon)` | $O(N^2 \log N + K)$ | $O(N^2)$ |

入力の検査は $O(N \log N)$、各始点での方向ソートと回転スイープは $O(N \log N)$ である。実装は最大辺数ぶんを戻り値に `reserve` するため、確保量は $O(N^2)$ となる。

## 注意点

- 入力は有限な座標を持ち、頂点数 3 以上かつ面積が 0 でない単純多角形でなければならない。
- 頂点の順序は時計回り・反時計回りのどちらでもよい。
- 頂点数不足、非有限な座標、退化辺、面積 0、自己交差に対しては `std::invalid_argument` を送出する。
- 幾何判定にはライブラリ共通の許容誤差を用いる。
