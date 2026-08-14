---
title: Timestamp MEX Marker (世代印MEXマーカー)
documentation_of: ../../../../src/structure/ordered_set/integer/mex_marker.hpp
---

小さな整数集合を何度も作り直して MEX を取るための世代印配列である。毎回の配列初期化を避けられる。

## constructor / universe

```cpp
explicit MexMarker(size_t universe)
size_t universe() const
```

領域 `[0,universe)` を用意し、その長さを返す。

## mark / marked

```cpp
void mark(size_t value)
bool marked(size_t value) const
```

現在世代で値を印付けし、印の有無を返す。

## clear / mex

```cpp
void clear()
size_t mex() const
```

`clear` は世代を進めて全印を論理削除する。`mex` は現在世代の最小未印値を返し、全値が印済みなら `universe` を返す。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| constructor | $O(U)$ | $O(U)$ |
| `mark`, `marked`, 通常の `clear` | $O(1)$ | $O(1)$ |
| 世代番号 overflow 時の `clear` | $O(U)$ | $O(1)$ |
| `mex` | $O(答え+1)$ | $O(1)$ |

## 注意点

値は `[0,U)` を要求し、違反時は `out_of_range` を送出する。`clear` 後の印は保持されない。
