---
title: Orthogonal Range Reporting (直交範囲列挙)
documentation_of: ../../../../src/structure/range_query/value_query/orthogonal_range_reporting.hpp
---

静的な二次元点集合から、半開長方形
$[x_l,x_r)\times[y_l,y_r)$ 内にある点の入力添字を列挙する。
$x$ 座標順のセグメント木の各節点に、$y$ 座標順の配列を保持する。

## API

```cpp
OrthogonalRangeReporting<Coordinate> index(points);
```

`points[i]` は入力添字 $i$ の `pair<Coordinate, Coordinate>` である。
独立した索引を構築し、入力配列は保持しない。

```cpp
int index.size() const noexcept;
```

点数を返す。

```cpp
vector<int> index.report(
    const Coordinate& lower_x,
    const Coordinate& upper_x,
    const Coordinate& lower_y,
    const Coordinate& upper_y
) const;
```

半開長方形内の入力添字を返す。境界が等しい空長方形では空配列を返す。

## 時間計算量

点数を $N$、報告点数を $K$ とする。

- コンストラクタ: $O(N\log N)$ 時間、$O(N\log N)$ 空間
- `size`: $O(1)$
- `report`: $O(\log^2 N+K)$ 時間、返り値を除く追加空間 $O(1)$

## 注意点

同一点も別の入力添字として保持する。出力順は保証しない。
`upper_x < lower_x` または `upper_y < lower_y` なら
`invalid_argument`、点数が `int` の範囲を超える場合は `length_error` を送出する。`Coordinate` は厳密弱順序を与える
`operator<` を持たなければならない。
