---
title: Dynamic Rectangle Add Point Get (動的長方形加算一点取得)
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

## API別の時間計算量・空間計算量

$R$ を `reserve_rectangle` した非退化長方形数とする。

| API | 時間計算量 |
| --- | --- |
| `DynamicRectangleAddPointGet()` | $O(1)$ |
| `reserve_rectangle(...)` | 償却 $O(1)$ |
| `build()` | $O(R\log^2 R)$（前処理） |
| `add(...)` | $O(\log^2 R)$ |
| `get(x,y)` | $O(\log^2 R)$ |

`build` 後の保持領域は $O(R\log R)$。空の長方形の予約・更新は $O(1)$。
## 注意点

- `reserve_rectangle(x1,y1,x2,y2)` は将来更新する半開矩形を登録する。
  空矩形は無視し、重複登録してよい。
- `build()` は登録矩形の4隅を圧縮する。build後の再予約・再build、
  build前のupdate/queryは例外。
- `add(x1,y1,x2,y2,value)` は予約済み矩形へ `value` を加える。
  空矩形では何もしない。未予約の非空矩形は例外。
- `get(x,y)` は点を含む全更新値の和。query点は事前予約不要。
