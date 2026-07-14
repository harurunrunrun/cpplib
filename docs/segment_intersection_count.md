---
title: Segment Intersection Count (線分交差対数)
documentation_of: ../src/algorithm/geometry/2d/segment_intersection_count.hpp
---

交差する線分の非順序対の個数を平面走査で数える。

## API

~~~cpp
std::size_t segment_intersection_count(
    const std::vector<Segment>& segments
);
~~~

一点での交差、端点共有、長さ0の線分との交差、
同一直線上での正の長さを持つ重なりを、いずれも線分対ごとに1回数える。

## API別の時間計算量・空間計算量

$N$ を線分数、$K$ を交差する線分対数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| segment_intersection_count(segments) | $O((N+K)\log N)$ | $O(N+K)$ |

個数だけを返すが、現在の実装は各交差対を重複なく管理するため
$O(N+K)$ の領域を用いる。

## 注意点

座標は有限で、shear変換と幾何演算が有限な long double の範囲に
収まる必要がある。条件を満たさない場合は std::invalid_argument
または std::overflow_error を送出する。
