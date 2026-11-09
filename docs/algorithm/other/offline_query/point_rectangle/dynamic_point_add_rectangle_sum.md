---
title: Dynamic Point Add Rectangle Sum (動的点加算長方形和)
documentation_of: ../../../../../src/algorithm/other/offline_query/point_rectangle/dynamic_point_add_rectangle_sum.hpp
---

点への加算と長方形内の和を処理する。更新に現れる点を事前に `reserve_point` で登録してから `build` する（Fenwick 木の各ノードに y 座標の Fenwick 木を持つ 2 次元 BIT）。

## 使い方

```cpp
DynamicPointAddRectangleSum<T, Coordinate = long long> seg;
seg.reserve_point(x, y);
seg.build();
seg.add(x, y, value);
seg.prefix_sum_less(x, y);
seg.prefix_sum_leq(x, y);
seg.sum(x1, y1, x2, y2);
```

## API

- `reserve_point(x, y)`: 将来 `add` する座標を登録する。重複登録してよい。
- `build()`: 登録座標を圧縮し、更新できる状態にする。
- `add(x, y, value)`: 予約済みの点 $(x, y)$ に `value` を加える。点 $(x, y)$ が予約されていなければ例外を送出する。
- `prefix_sum_less(x, y)`: $p_x<x$ かつ $p_y<y$ を満たす点の重みの和を返す。
- `prefix_sum_leq(x, y)`: $p_x\le x$ かつ $p_y\le y$ を満たす点の重みの和を返す。
- `sum(x1, y1, x2, y2)`: 半開長方形 $[x_1, x_2) \times [y_1, y_2)$ 内の重みの和を返す。`prefix_sum_less` を 4 回呼ぶ。

クエリの座標は予約しなくてよい。

## 計算量

$N$ を `reserve_point` した点数とする。

| API | 時間計算量 |
| --- | --- |
| `DynamicPointAddRectangleSum()` | $O(1)$ |
| `reserve_point(x, y)` | 償却 $O(1)$ |
| `build()` | $O(N\log^2 N)$ |
| `add(x, y, value)` | $O(\log^2 N)$ |
| `prefix_sum_less(x, y)` / `prefix_sum_leq(x, y)` | $O(\log^2 N)$ |
| `sum(x1, y1, x2, y2)` | $O(\log^2 N)$ |

`build` 後の保持領域は $O(N\log N)$。

## 注意点

- `build` 後の `reserve_point` と再度の `build`、`build` 前の `add` とクエリは例外を送出する。
- `sum` は端点の順序を検査しない。$x_1\le x_2$ かつ $y_1\le y_2$ を前提とする。
