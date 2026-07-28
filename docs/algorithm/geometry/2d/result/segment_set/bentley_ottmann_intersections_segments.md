---
title: Bentley Ottmann Intersections from Segments (線分列のBentley–Ottmann交差列挙)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/segment_set/bentley_ottmann_intersections_segments.hpp
---

Bentley–Ottmann法で、交差する全線分対と交差形状を列挙する。

## API

```cpp
std::vector<SegmentIntersectionRecord> bentley_ottmann_intersections(
    const std::vector<Segment>& segments
);
```

結果は `(first, second)` の辞書順で、同じ線分対を一度だけ含む。垂直線分、
長さ0の線分、端点共有、多重点、同一直線上の重複を扱う。

## 時間計算量

線分数を $N$、交差する線分対数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `bentley_ottmann_intersections(segments)` | $O((N+K)\log N)$ | $O(N+K)$ |

shear方向の選択は $O(N)$ であり、全線分対の総当たりは行わない。

## 注意点

座標は有限でなければならない。shear変換または幾何演算が有限な
`long double` の範囲を超える場合は `std::invalid_argument` または
`std::overflow_error` を送出する。交差判定は幾何ライブラリ共通の許容誤差を使う。

## 使用例

```cpp
std::vector<Segment> segments{
    Segment{Point{0, 0}, Point{2, 2}},
    Segment{Point{0, 2}, Point{2, 0}},
};
const auto records = bentley_ottmann_intersections(segments);
```
