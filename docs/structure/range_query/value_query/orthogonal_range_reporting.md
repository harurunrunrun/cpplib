---
title: Orthogonal Range Reporting (直交範囲列挙)
documentation_of: ../../../../src/structure/range_query/value_query/orthogonal_range_reporting.hpp
---

静的な 2 次元の点集合から、半開の長方形 $[x_l,x_r)\times[y_l,y_r)$ 内にある点の入力時の添字を列挙する。$x$ 座標順のセグメント木の各節点に、$y$ 座標順の配列を保持する。

## API

```cpp
template<class Coordinate>
class OrthogonalRangeReporting;

OrthogonalRangeReporting<Coordinate> index(points);
```

`points[i]` は入力時の添字が $i$ の点を表す `pair<Coordinate, Coordinate>` である。独立した索引を構築し、入力配列は保持しない。

```cpp
int index.size() const noexcept;
```

点の個数を返す。

```cpp
vector<int> index.report(
    const Coordinate& lower_x,
    const Coordinate& upper_x,
    const Coordinate& lower_y,
    const Coordinate& upper_y
) const;
```

半開の長方形 `[lower_x, upper_x) x [lower_y, upper_y)` 内の点の入力時の添字を返す。境界が等しい空の長方形では空の配列を返す。

## 時間計算量

点の個数を $N$、報告する点の個数を $K$ とする。

- コンストラクタ: $O(N\log N)$ 時間、$O(N\log N)$ 空間
- `size`: $O(1)$
- `report`: $O(\log^2 N+K)$ 時間、戻り値を除く追加空間 $O(1)$

## 注意点

- 同じ座標の点も、別の入力時の添字として保持する。出力の順序は保証しない。
- `upper_x < lower_x` または `upper_y < lower_y` なら `std::invalid_argument` を送出する。点の個数が `int` の範囲を超える場合は `std::length_error` を送出する。
- `Coordinate` は狭義弱順序を与える `operator<` を持たなければならない。
