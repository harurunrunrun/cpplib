---
title: Minimum Enclosing Circle Result (最小包含円の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/minimum_enclosing_circle_result.hpp
---

最小包含円を決定する支持点（support）と、円に対する点の位置判定 API を保持する結果型。

## `MinimumEnclosingCircleContainment`

```cpp
enum class MinimumEnclosingCircleContainment : int {
    OUTSIDE = -1,
    ON_BOUNDARY = 0,
    INSIDE = 1,
};
```

クエリ点が円の外部、円周上、内部のどこにあるかを表す。

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

- `support[0]` から `support[support_size - 1]`: 円を決定する $0$ 点から $3$ 点の支持点。
- `support_size`: 有効な支持点の個数。デフォルト構築では $0$。
- `containment(point)`: 円に対する位置を 3 値で返す。
- `contains(point)`: 内部または円周上なら `true` を返す。
- `on_boundary(point)`: 円周上なら `true` を返す。
- `circle()`: 支持点から中心と半径を構築する（2 点なら直径の円、3 点なら外接円）。

## 時間計算量

| API・操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| デフォルト構築、各フィールドの参照 | $O(1)$ | $O(1)$ |
| `containment` / `contains` / `on_boundary` | $O(1)$ | $O(1)$ |
| `circle` | $O(1)$ | $O(1)$ |

## 注意点

- `support_size == 0` では全点を外部と判定し、`circle()` は中心が原点・半径 $0$ の円を返す。
- `support_size` は $0$ から $3$ でなければならない。
- 中心または半径を有限な `long double` で表せない場合は `std::overflow_error` を送出する。
- 距離の比較には、ライブラリ共通の許容誤差と丸め誤差の項を用いる。
