---
title: Timestamp MEX Marker (世代印MEXマーカー)
documentation_of: ../../../../src/structure/ordered_set/integer/mex_marker.hpp
---

小さな整数集合を何度も作り直しながら MEX を求めるための世代印（タイムスタンプ）配列である。集合を作り直すたびに配列を初期化する必要がない。

## コンストラクタ / universe

```cpp
explicit MexMarker(size_t universe)
size_t universe() const
```

コンストラクタは値の領域 $[0, U)$（$U$ = `universe`）を用意する。`universe()` は $U$ を返す。

## mark / marked

```cpp
void mark(size_t value)
bool marked(size_t value) const
```

`mark` は現在の世代で値 `value` に印を付け、`marked` は現在の世代で `value` に印が付いているかを返す。

## clear / mex

```cpp
void clear()
size_t mex() const
```

`clear` は世代を進めることで、すべての印を論理的に削除する。`mex` は現在の世代で印の付いていない最小の値を返し、すべての値に印が付いていれば $U$ を返す。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(U)$ | $O(U)$ |
| `mark`, `marked`, 通常の `clear` | $O(1)$ | $O(1)$ |
| 世代番号がオーバーフローするときの `clear` | $O(U)$ | $O(1)$ |
| `mex` | $O(\mathrm{mex}+1)$ | $O(1)$ |

## 注意点

`mark` と `marked` は値が $[0, U)$ にあることを要求し、範囲外なら `std::out_of_range` を送出する。`clear` の後には以前の印は残らない。
