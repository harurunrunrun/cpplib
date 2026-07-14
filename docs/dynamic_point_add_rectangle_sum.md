---
title: Dynamic Point Add Rectangle Sum (動的点加算長方形和) [GIVEAWAY] [point_add_rectangle_sum] [RACETIME]
documentation_of: ../src/algorithm/other/dynamic_point_add_rectangle_sum.hpp
---

点加算・長方形和。

更新に現れる点を `reserve_point` で登録してから `build` する。

# コンストラクタ

```cpp
DynamicPointAddRectangleSum<T, Coordinate> seg
```

# 関数

```cpp
seg.reserve_point(x, y)
seg.build()
seg.add(x, y, value)
seg.prefix_sum_less(x, y)
seg.prefix_sum_leq(x, y)
seg.sum(x1, y1, x2, y2)
```

`sum(x1, y1, x2, y2)` は半開長方形 $[x1, x2) \times [y1, y2)$ の和を返す。

## 時間計算量

- `build`: $O(N \log^2 N)$
- `add`, `sum`: $O(\log^2 N)$

## API別の時間計算量・空間計算量

$N$ を `reserve_point` した点数とする。

| API | 時間計算量 |
| --- | --- |
| `DynamicPointAddRectangleSum()` | $O(1)$ |
| `reserve_point(x,y)` | 償却 $O(1)$ |
| `build()` | $O(N\log^2 N)$（前処理） |
| `add(x,y,value)` | $O(\log^2 N)$ |
| `prefix_sum_less(x,y)` / `prefix_sum_leq(x,y)` | $O(\log^2 N)$ |
| `sum(x1,y1,x2,y2)` | $O(\log^2 N)$ |

`sum` は4回のprefix queryを行う。`build` 後の保持領域は $O(N\log N)$。
## 注意点

- `reserve_point(x,y)` は将来 `add` する座標を登録する。重複登録してよい。
- `build()` は登録座標を圧縮して更新可能状態にする。
  build後の再予約・再build、build前のquery/updateは例外。
- `add(x,y,value)` は予約済みの点へ `value` を加える。
  xまたはyが未予約なら例外。
- `prefix_sum_less(x,y)` は $p_x<x,p_y<y$、
  `prefix_sum_leq(x,y)` は $p_x\le x,p_y\le y$ の重み和。
  query座標自体は予約不要。
- `sum(x1,y1,x2,y2)` は半開矩形の和。端点順序は検査せず、
  $x1\le x2,y1\le y2$ を前提とする。
