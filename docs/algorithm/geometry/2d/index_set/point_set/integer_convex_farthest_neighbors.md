---
title: Integer Convex Farthest Neighbors (整数凸多角形の全頂点の最遠頂点) [all_furthest_neighbors]
documentation_of: ../../../../../../src/algorithm/geometry/2d/index_set/point_set/integer_convex_farthest_neighbors.hpp
---

凸多角形の各入力位置について、最も遠い入力位置を返す。

## API

```cpp
std::vector<int> integer_convex_farthest_neighbors(
    const std::vector<std::pair<long long, long long>>& points
);
```

`result[i]` は `points[i]` から最も遠い点の入力添字である。
正規化で頂点を削除しないので、時計回り・反時計回りのどちらの向きも、同一直線上の中間点や連続する重複頂点も、そのままの位置で扱える。
距離の 2 乗は符号付き 128 bit 整数で厳密に比較する。

周回列を 2 分割し、互いに交わらない 2 つの弧の間の距離行列（全単調行列）に SMAWK を適用して、分割統治で全行の最大値を求める。

## 時間計算量

$O(N \log N)$、空間 $O(N)$。

## 注意点

- 入力は凸多角形の弱凸な周回列であること。凸性の検査は行わない。
- 座標は $[-2^{62}, 2^{62})$ の範囲に制限する。範囲外の座標があれば `std::out_of_range` を送出する。
- 最遠の点が複数あれば、そのいずれかを返す。
- 空列・1 点・2 点も扱える。頂点数は `int` で表せる範囲であること。
