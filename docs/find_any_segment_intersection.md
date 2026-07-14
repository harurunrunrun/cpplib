---
title: Find Any Segment Intersection (任意の線分交差)
documentation_of: ../src/algorithm/geometry/2d/find_any_segment_intersection.hpp
---

交差する線分対が存在するかを平面走査で調べ、見つかった一組を返す。

## API

~~~cpp
std::optional<std::pair<std::size_t, std::size_t>>
find_any_segment_intersection(const std::vector<Segment>& segments);
~~~

交差があれば first < second を満たす入力添字の組を返し、
なければ std::nullopt を返す。端点共有、長さ0の線分、
同一直線上の重なりも交差として扱う。

## API別の時間計算量・空間計算量

$N$ を線分数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| find_any_segment_intersection(segments) | $O(N\log N)$ | $O(N)$ |

## 注意点

座標は有限で、shear変換と幾何演算が有限な long double の範囲に
収まる必要がある。条件を満たさない場合は std::invalid_argument
または std::overflow_error を送出する。
