---
title: Axis Aligned Segment Intersections (軸平行線分交差) [CGL_6_A]
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/segment_set/axis_aligned_segment_intersections.hpp
---

軸に平行な線分の集合について、水平な線分と垂直な線分が交差する組の個数を数える。

## API

```cpp
long long count_axis_aligned_segment_intersections(
    const std::vector<Segment>& segments
);
```

端点での交差も数える。同じ線分が複数回与えられた場合も、線分の組ごとに数える。平行な線分どうしの重なりは数えない。

長さ $0$ の線分、または軸に平行でない線分が含まれる場合は `std::invalid_argument` を送出する。

## 時間計算量

線分の数を $N$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_axis_aligned_segment_intersections(segments)` | $O(N \log N)$ | $O(N)$ |

イベントの生成と座標圧縮が前処理で $O(N \log N)$、各イベントでの Fenwick 木の更新・クエリが $O(\log N)$ である。交点そのものは列挙せず総数だけを返すため、交点数に依存する項はない。

## 注意点

- 長さ 0 か、軸に平行かの判定には `GEOMETRY_EPS` による許容誤差を用いる。一方、交差判定（イベントの $x$ 座標の比較や水平線分の $y$ 座標との照合）は誤差なしの比較で行う。
- 座標と中間演算の値は有限な `long double` の範囲に収まる必要がある。
