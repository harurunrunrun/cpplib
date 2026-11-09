---
title: Find Any Segment Intersection (任意の線分交差)
documentation_of: ../../../../../../src/algorithm/geometry/2d/index_set/segment_set/find_any_segment_intersection.hpp
---

交差する線分対が存在するかを平面走査で調べ、見つかった 1 組を返す。

## API

~~~cpp
std::optional<std::pair<std::size_t, std::size_t>>
find_any_segment_intersection(const std::vector<Segment>& segments);
~~~

交差があれば `first < second` を満たす入力添字の組を返し、なければ `std::nullopt` を返す。
端点の共有、長さ 0 の線分、同一直線上の重なりも交差として扱う。
内部では Bentley–Ottmann 法の走査を、最初の交差対が見つかった時点で打ち切る。

## API別の時間計算量・空間計算量

$N$ を線分数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `find_any_segment_intersection(segments)` | $O(N \log N)$ | $O(N)$ |

## 注意点

座標は有限で、せん断変換と幾何演算が有限な `long double` の範囲に収まる必要がある。
条件を満たさない場合は `std::invalid_argument` または `std::overflow_error` を、線分数が多すぎてせん断係数を選べない場合は `std::length_error` を送出する。
