---
title: Axis Aligned Segment Intersections
documentation_of: ../src/algorithm/geometry/axis_aligned_segment_intersections.hpp
---

軸平行線分について、水平線分と垂直線分の交差対数を数える。

```cpp
long long count_axis_aligned_segment_intersections(const vector<Segment>& segments)
```

端点での交差を含む。同じ線分が複数回与えられた場合も線分対ごとに数える。平行な線分同士の重なりは数えない。

長さ $0$ の線分、軸に平行でない線分が含まれる場合は例外を送出する。

## 計算量

$N$ を線分数として $O(N\log N)$。

## API別計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_axis_aligned_segment_intersections(segments)` | $O(N\log N)$ | $O(N)$ |

event生成・座標圧縮が前処理 $O(N\log N)$、各eventのFenwick tree更新/queryが $O(\log N)$。交点自体は列挙せず総数だけを返すため、交点数への出力依存項はない。
