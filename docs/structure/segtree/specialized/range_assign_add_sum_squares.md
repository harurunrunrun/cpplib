---
title: Range Assign Add Sum of Squares (区間代入・加算・二乗和) [SEGSQRSS]
documentation_of: ../../../../src/structure/segtree/specialized/range_assign_add_sum_squares.hpp
---

区間代入と区間加算に対応し、区間内の要素の二乗和を返す。

## API

### `RangeAssignAddSumSquares<T, MAX_SIZE>(values)`

初期列から構築する。内部では `LazySegtree` を使い、`MAX_SIZE` を省略すると可変長版になって長さの上限がなくなる。

### `size()`

要素数を返す。

### `assign(left, right, value)`

半開区間 $[left, right)$ の全要素に `value` を代入する。

### `add(left, right, value)`

半開区間 $[left, right)$ の全要素に `value` を加える。

### `square_sum(left, right)`

半開区間 $[left, right)$ の $\sum a_i^2$ を返す。遅延作用を押し下げるため `const` ではない。

## API 別の時間計算量・空間計算量

$N$ を要素数とする。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(\texttt{MAX_SIZE})$ |
| `size()` | $O(1)$ | $O(1)$ |
| `assign` / `add` / `square_sum` | $O(\log N)$ | $O(1)$ |

## 注意点

- 固定容量版では $N \le \texttt{MAX_SIZE}$ が必要である。
- 区間は半開区間である。範囲違反では `std::runtime_error` を送出する。
- 和・二乗和・作用の途中の積が `T` で表現できなければならない。
