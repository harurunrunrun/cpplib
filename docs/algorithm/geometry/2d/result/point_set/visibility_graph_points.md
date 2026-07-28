---
title: Visibility Graph from Polygon Points (多角形頂点列の可視グラフ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/visibility_graph_points.hpp
---

単純多角形の全頂点を節点とする無向可視グラフを構築する。

## API

```cpp
std::vector<VisibilityGraphEdge> visibility_graph(
    const std::vector<Point>& polygon
);
```

隣接頂点は常に辺として含める。非隣接頂点は、その開線分が外部を通らず、
第3の多角形頂点も通らない場合に含める。戻り値は `(first, second)` の辞書順である。
各始点からの可視性は偏角順の回転sweepで一括判定する。

## 時間計算量

頂点数を $N$、返す辺数を $K$ とする。

| API | 時間計算量 | 空間計算量（返り値を含む） |
| --- | --- | --- |
| `visibility_graph(polygon)` | $O(N^2\log N + K)$ | $O(N^2)$ |

入力検査は $O(N\log N)$、各始点の方向ソートと回転sweepは $O(N\log N)$ である。
実装は最大辺数分を戻り値に `reserve` するため、確保量は $O(N^2)$ となる。

## 注意点

- 入力は有限座標を持つ、3頂点以上かつ面積非零の単純多角形でなければならない。
- 入力順は時計回り・反時計回りのどちらでもよい。
- 退化辺・自己交差には `std::invalid_argument` を送出する。
- 幾何判定にはライブラリ共通の許容誤差を用いる。
