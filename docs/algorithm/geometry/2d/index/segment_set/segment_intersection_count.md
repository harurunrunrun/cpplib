---
title: Segment Intersection Count (線分交差対数)
documentation_of: ../../../../../../src/algorithm/geometry/2d/index/segment_set/segment_intersection_count.hpp
---

交差する線分の非順序対の個数を、平面走査（Bentley–Ottmann 法）で数える。

## API

~~~cpp
std::size_t segment_intersection_count(
    const std::vector<Segment>& segments
);
~~~

1 点での交差、端点の共有、長さ 0 の線分との交差、同一直線上での正の長さを持つ重なりを、いずれも線分対ごとに 1 回だけ数える。

## API別の時間計算量・空間計算量

$N$ を線分数、$K$ を交差する線分対の数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `segment_intersection_count(segments)` | $O((N + K) \log N)$ | $O(N + K)$ |

個数だけを返すが、現在の実装は `bentley_ottmann_intersections(segments).size()` を返すので、交差対をすべて列挙して $O(N + K)$ の領域を使う。

## 注意点

座標は有限で、せん断変換と幾何演算が有限な `long double` の範囲に収まる必要がある。
条件を満たさない場合は `std::invalid_argument` または `std::overflow_error` を、線分数が多すぎてせん断係数を選べない場合は `std::length_error` を送出する。
