---
title: Grid Forest Diameter (格子森の直径) [LABYR1]
documentation_of: ../../../../../src/algorithm/graph/specialized/metric_analysis/grid_forest_diameter.hpp
---

4 近傍の格子で、通行可能なマスが作る森の各連結成分の直径を求め、その最大値を返す。

## API

### `grid_forest_diameter`

```cpp
int grid_forest_diameter(
    const std::vector<std::string>& grid,
    char traversable_cell = '.',
    char blocked_cell = '#'
);
```

`traversable_cell` のマスどうしを上下左右に接続した無向グラフを考え、その連結成分の直径の最大値を辺数で返す。通行可能なマスが存在しない場合は $0$ を返す。`blocked_cell` のマスは頂点に含めない。

## 計算量

行数を $R$、列数を $C$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `grid_forest_diameter` | $O(RC)$ | $O(RC)$ |

## 注意点

- `grid` は長方形で、各マスは `traversable_cell` または `blocked_cell` でなければならない。
- 2 種類のマスの記号は互いに異なっていなければならない。
- 通行可能なマスが作る各連結成分は木でなければならない。
- 格子の形状・記号の前提違反や閉路の検出時には `invalid_argument` を、内部の添字を `int` で表せない大きさの格子には `length_error` を送出する。
