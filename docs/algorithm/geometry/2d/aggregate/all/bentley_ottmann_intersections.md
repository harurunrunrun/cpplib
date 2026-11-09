---
title: Bentley Ottmann Intersections Aggregator (Bentley–Ottmann交差列挙集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/bentley_ottmann_intersections.hpp
---

交差記録の型と、線分列を入力とする API をまとめて読み込む後方互換の集約ヘッダである。Bentley-Ottmann 法により、交差する線分の組とその交差の形状を列挙する。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `segment_intersection_record.hpp` | `SegmentIntersectionRecord` |
| `bentley_ottmann_intersections_segments.hpp` | `bentley_ottmann_intersections(segments)` |

必要な leaf header だけを直接 include することもできる。従来どおり `bentley_ottmann_intersections.hpp` を include すれば全 API を使える。

## API

```cpp
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
```

`first < second` であり、同じ線分の組は 1 度だけ返す。1 点で交わるときは `kind == SEGMENT_INTERSECTION_POINT` で、`first_point == second_point` となる。正の長さを持つ区間で重なるときは `kind == SEGMENT_INTERSECTION_SEGMENT` で、その両端を `first_point`, `second_point` に返す。結果は `(first, second)` の辞書順に並ぶ。

垂直な線分、長さ $0$ の線分、端点の共有、多数の線分が同一点で交わる場合、同一直線上の重なりも扱う。

## 計算量

$N$ を線分の本数、$K$ を交差する線分の組の個数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `bentley_ottmann_intersections(segments)` | $O((N+K)\log N)$ | $O(N+K)$ |

せん断（shear）方向の選択は $O(N)$ であり、全線分の組の総当たりは行わない。

## 注意点

- 座標は有限でなければならない。せん断変換や幾何演算が有限な `long double` の範囲を超える入力には、`std::invalid_argument` または `std::overflow_error` を送出する。
- せん断方向の選択に使える線分数を超える場合は `std::length_error` を送出する。
- 交差判定には 2 次元幾何で共通の許容誤差を用いる。
