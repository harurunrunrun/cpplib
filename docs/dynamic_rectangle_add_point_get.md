---
title: Dynamic Rectangle Add Point Get
documentation_of: ../src/algorithm/other/dynamic_rectangle_add_point_get.hpp
---

長方形加算・点取得。

更新に現れる長方形を `reserve_rectangle` で登録してから `build` する。

# コンストラクタ

```cpp
DynamicRectangleAddPointGet<T, Coordinate> seg
```

# 関数

```cpp
seg.reserve_rectangle(x1, y1, x2, y2)
seg.build()
seg.add(x1, y1, x2, y2, value)
seg.get(x, y)
```

長方形は半開領域 $[x1, x2) \times [y1, y2)$ として扱う。

## 時間計算量

- `build`: $O(N \log^2 N)$
- `add`, `get`: $O(\log^2 N)$

## API別計算量

$R$ を `reserve_rectangle` した非退化長方形数とする。

| API | 時間計算量 |
| --- | --- |
| `DynamicRectangleAddPointGet()` | $O(1)$ |
| `reserve_rectangle(...)` | 償却 $O(1)$ |
| `build()` | $O(R\log^2 R)$（前処理） |
| `add(...)` | $O(\log^2 R)$ |
| `get(x,y)` | $O(\log^2 R)$ |

`build` 後の保持領域は $O(R\log R)$。空の長方形の予約・更新は $O(1)$。
