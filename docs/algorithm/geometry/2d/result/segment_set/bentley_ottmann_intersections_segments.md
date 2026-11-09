---
title: Bentley Ottmann Intersections from Segments (線分列の Bentley–Ottmann 交差列挙)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/segment_set/bentley_ottmann_intersections_segments.hpp
---

Bentley–Ottmann 法で、交差する線分の組をすべて列挙し、それぞれの交差の形状を返す。

## API

```cpp
std::vector<SegmentIntersectionRecord> bentley_ottmann_intersections(
    const std::vector<Segment>& segments
);
```

各要素 `SegmentIntersectionRecord` は線分の添字 `first < second`、交差の種類 `kind`（`SEGMENT_INTERSECTION_POINT` または `SEGMENT_INTERSECTION_SEGMENT`）、交差部分の端点 `first_point`, `second_point` を持つ。結果は `(first, second)` の辞書順に並び、同じ線分の組は 1 度だけ含む。垂直な線分、長さ 0 の線分、端点の共有、多重交点、同一直線上での重なりを扱う。

## 時間計算量

線分の数を $N$、交差する線分の組の数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `bentley_ottmann_intersections(segments)` | $O((N + K) \log N)$ | $O(N + K)$ |

スイープ方向を決める剪断（shear）変換の係数の選択は $O(N)$ であり、全線分対の総当たりは行わない。

## 注意点

- 座標は有限でなければならず、そうでなければ `std::invalid_argument` を送出する。
- 剪断変換や幾何演算が有限な `long double` の範囲を超えるおそれがある場合は `std::overflow_error` を、数値的に垂直でないスイープ方向を選べない場合は `std::invalid_argument` を送出する。
- 交差判定には幾何ライブラリ共通の許容誤差を用いる。

## 使用例

```cpp
std::vector<Segment> segments{
    Segment{Point{0, 0}, Point{2, 2}},
    Segment{Point{0, 2}, Point{2, 0}},
};
const auto records = bentley_ottmann_intersections(segments);
```
