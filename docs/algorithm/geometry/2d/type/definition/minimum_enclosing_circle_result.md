---
title: Minimum Enclosing Circle Result (最小包含円の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/minimum_enclosing_circle_result.hpp
---

最小包含円を決定するsupportと、円に対する点の位置判定APIを保持する結果型。

## `MinimumEnclosingCircleContainment`

```cpp
enum class MinimumEnclosingCircleContainment : int {
    OUTSIDE = -1,
    ON_BOUNDARY = 0,
    INSIDE = 1,
};
```

問い合わせ点が円の外部、円周上、内部のどこにあるかを表す。

## `MinimumEnclosingCircleResult`

```cpp
struct MinimumEnclosingCircleResult {
    std::array<Point, 3> support;
    std::size_t support_size;

    MinimumEnclosingCircleContainment containment(const Point& point) const;
    bool contains(const Point& point) const;
    bool on_boundary(const Point& point) const;
    Circle circle() const;
};
```

- `support[0..support_size)`: 円を決定する0点から3点。
- `support_size`: 有効なsupport数。
- `containment(point)`: 円に対する位置を3値で返す。
- `contains(point)`: 内部または円周上ならtrue。
- `on_boundary(point)`: 円周上ならtrue。
- `circle()`: supportから中心と半径を構築する。

## 時間計算量

| API・操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 既定構築、各fieldの参照 | $O(1)$ | $O(1)$ |
| `containment` / `contains` / `on_boundary` | $O(1)$ | $O(1)$ |
| `circle` | $O(1)$ | $O(1)$ |

## 注意点

- `support_size == 0` では全点を外部と判定し、`circle()` は中心原点・半径0を返す。
- `support_size` は0から3でなければならない。
- 中心または半径を有限な `long double` で表せない場合は `std::overflow_error` を送出する。
- 距離比較にはライブラリ共通の許容誤差と丸め誤差項を用いる。
