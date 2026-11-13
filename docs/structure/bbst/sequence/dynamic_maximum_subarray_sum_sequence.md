---
title: Dynamic Maximum Subarray Sum Sequence (動的列最大部分配列和) [GSS6]
documentation_of: ../../../../src/structure/bbst/sequence/dynamic_maximum_subarray_sum_sequence.hpp
---

挿入・削除・一点代入に対応する列で、指定区間の非空な部分配列の和の最大値を返す。内部では `ReversibleSplayTree` を使う。

## テンプレート引数

```cpp
DynamicMaximumSubarraySumSequence<T, MAX_SIZE = -1>
```

`MAX_SIZE` は同時に保持する要素数の上限で、指定するとノード領域を固定確保する。省略（`-1`）するとノードを `std::vector` に必要に応じて確保する可変長版になる。

## API

### `DynamicMaximumSubarraySumSequence(values)`

初期列 `values`（`std::vector<T>`）から構築する。

### `size()` / `empty()`

現在の要素数と、空かどうかを返す。

### `insert(position, value)`

位置 `position` の直前に値を挿入する（`position == size()` なら末尾）。

### `erase(position)`

位置 `position` の要素を削除する。

### `set(position, value)`

位置 `position` に値を代入する。

### `maximum_sum(left, right)`

非空の半開区間 $[\mathrm{left},\mathrm{right})$ に含まれる、非空の部分配列の和の最大値を返す。

## 時間計算量

要素数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| コンストラクタ | 固定長版 $O(\texttt{MAX_SIZE})$、可変長版 $O(N)$ | 固定長版 $O(\texttt{MAX_SIZE})$、可変長版 $O(N)$ |
| `size()` / `empty()` | $O(1)$ | $O(1)$ |
| `insert` / `erase` / `set` | 償却 $O(\log N)$ | $O(1)$ |
| `maximum_sum` | 償却 $O(\log N)$ | $O(1)$ |

## 注意点

- 固定長版では、同時に保持する要素数が `MAX_SIZE` 以下でなければならない。
- `maximum_sum` は非空区間だけを受け付け、`right <= left` なら `std::runtime_error` を投げる。
- 添字・容量の違反でも `std::runtime_error` を投げる。
