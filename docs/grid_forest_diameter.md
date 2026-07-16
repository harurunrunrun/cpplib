---
title: Grid Forest Diameter (格子森の直径) [LABYR1]
documentation_of: ../src/algorithm/graph/grid_forest_diameter.hpp
---

4近傍格子で、通行可能マスが作る森の各連結成分の直径を求め、その最大値を返します。

## API

### `grid_forest_diameter(grid, traversable_cell, blocked_cell)`

```cpp
int grid_forest_diameter(
    const vector<string>& grid,
    char traversable_cell = '.',
    char blocked_cell = '#'
);
```

`traversable_cell` 同士を上下左右に接続した無向グラフを作り、その最大直径を
辺数で返します。通行可能マスが存在しない場合は `0` です。
`blocked_cell` は頂点に含めません。

## API別の時間計算量・空間計算量

行数を $R$、列数を $C$ とします。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `grid_forest_diameter` | $O(RC)$ | $O(RC)$ |

## 注意点

- `grid` は長方形で、各マスは `traversable_cell` または
  `blocked_cell` でなければなりません。
- 2種類のマス記号は互いに異なる必要があります。
- 通行可能マスが作る各連結成分は木でなければなりません。閉路を検出した場合も
  `invalid_argument` を送出します。
- 格子の形状・記号・森の前提違反には `invalid_argument`、内部添字を
  `int` で表せない大きさには `length_error` を送出します。
