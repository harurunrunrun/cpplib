---
title: Dynamic Rectangle Add Point Get (動的長方形加算一点取得)
documentation_of: ../../../../../src/algorithm/other/offline_query/point_rectangle/dynamic_rectangle_add_point_get.hpp
---

長方形への加算と一点の値の取得を処理する。更新に現れる長方形を事前に `reserve_rectangle` で登録してから `build` する。内部では長方形の 4 隅への点加算として `DynamicPointAddRectangleSum` に帰着する。

## 使い方

```cpp
DynamicRectangleAddPointGet<T, Coordinate = long long> seg;
seg.reserve_rectangle(x1, y1, x2, y2);
seg.build();
seg.add(x1, y1, x2, y2, value);
seg.get(x, y);
```

長方形は半開領域 $[x_1, x_2) \times [y_1, y_2)$ として扱う。各軸の端点が逆順なら入れ替えて扱う。

## API

- `reserve_rectangle(x1, y1, x2, y2)`: 将来更新する半開長方形を登録する。空の長方形は無視する。重複登録してよい。
- `build()`: 登録した長方形の 4 隅を座標圧縮する。
- `add(x1, y1, x2, y2, value)`: 予約済みの長方形に `value` を加える。空の長方形では（`build` 前でも）何もしない。未予約の空でない長方形では例外を送出する。
- `get(x, y)`: 点 $(x, y)$ を含むすべての更新値の和を返す。クエリの点は事前に予約しなくてよい。

## 計算量

$R$ を `reserve_rectangle` した空でない長方形の数とする。

| API | 時間計算量 |
| --- | --- |
| `DynamicRectangleAddPointGet()` | $O(1)$ |
| `reserve_rectangle(...)` | 償却 $O(1)$ |
| `build()` | $O(R\log^2 R)$ |
| `add(...)` | $O(\log^2 R)$ |
| `get(x, y)` | $O(\log^2 R)$ |

`build` 後の保持領域は $O(R\log R)$。空の長方形の予約・更新は $O(1)$。

## 注意点

- `build` 後の空でない長方形の予約と再度の `build`、`build` 前の空でない長方形への `add` と `get` は例外を送出する。
- `T` は加算・減算・単項マイナスを持たなければならない。
