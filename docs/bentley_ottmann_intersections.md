---
title: Bentley Ottmann Intersections (Bentley–Ottmann交差列挙)
documentation_of: ../src/algorithm/geometry/2d/bentley_ottmann_intersections.hpp
---

Bentley–Ottmann法によって、交差する線分対とその交差形状を列挙する。

## API

~~~cpp
struct SegmentIntersectionRecord {
    std::size_t first;
    std::size_t second;
    SegmentIntersectionKind kind;
    Point first_point;
    Point second_point;
};

std::vector<SegmentIntersectionRecord> bentley_ottmann_intersections(
    const std::vector<Segment>& segments
);
~~~

first < second であり、同じ線分対は一度だけ返す。
一点で交わるとき kind == SEGMENT_INTERSECTION_POINT で
first_point == second_point となる。正の長さを持つ区間で重なるとき
kind == SEGMENT_INTERSECTION_SEGMENT で、その両端を
first_point, second_point に返す。結果は (first, second) の辞書順である。

垂直線分、長さ0の線分、端点共有、多数の線分が同一点で交わる場合、
および同一直線上の重なりを扱う。

## API別の時間計算量・空間計算量

$N$ を線分数、$K$ を交差する線分対数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| bentley_ottmann_intersections(segments) | $O((N+K)\log N)$ | $O(N+K)$ |

shear方向の選択は $O(N)$ であり、全線分対の総当たりは行わない。

## 注意点

座標は有限でなければならず、shear変換と幾何演算が有限な long double
の範囲を超える入力には std::invalid_argument または
std::overflow_error を送出する。交差判定は2次元幾何共通の許容誤差を用いる。
