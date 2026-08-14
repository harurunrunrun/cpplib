---
title: Interval Update MEX Set (区間更新MEX集合)
documentation_of: ../../../src/structure/interval/interval_mex_set.hpp
---

巨大な有限整数領域に対する区間 set、clear、flip と MEX を implicit lazy segment tree で処理する。

## constructor

```cpp
explicit IntervalMexSet(uint64_t universe)
```

`[0,universe)` がすべて欠損した状態を作る。

## assign / flip

```cpp
void assign(uint64_t left, uint64_t right, bool present)
void flip(uint64_t left, uint64_t right)
```

半開区間を一括して存在・欠損にするか、存在状態を反転する。

## mex

```cpp
uint64_t mex()
```

最小欠損値を返す。領域が完全なら `universe` を返す。

## universe / distinct_size / node_count

領域長、存在値数、現在確保した node 数を $O(1)$ で返す。

## 時間計算量

$C=universe$、更新で生成される node 数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| constructor | $O(1)$ | $O(1)$ |
| `assign`, `flip` | $O(\log C)$ | 償却 $O(\log C)$ |
| `mex` | $O(\log C)$ | 償却 $O(\log C)$ |
| getter | $O(1)$ | $O(1)$ |
| 全体 | - | $O(K)$ |

## 注意点

`universe > 0` が必要である。区間は `0 <= left <= right <= universe` を要求し、違反時は例外を送出する。
