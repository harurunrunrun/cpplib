---
title: Dynamic Maximum Subarray Sum Sequence (動的列最大部分配列和) [GSS6]
documentation_of: ../src/structure/bbst/dynamic_maximum_subarray_sum_sequence.hpp
---

挿入・削除・一点代入に対応する列で、非空区間の最大部分配列和を返します。

## API

### `DynamicMaximumSubarraySumSequence(values)`

初期列から構築します。

### `size()` / `empty()`

現在の要素数、空かどうかを返します。

### `insert(position, value)`

`position` の直前へ値を挿入します。

### `erase(position)`

指定位置を削除します。

### `set(position, value)`

指定位置を代入します。

### `maximum_sum(left, right)`

非空半開区間 `[left,right)` の最大部分配列和を返します。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N)$ | $O(MAX\_SIZE)$ |
| `size()` / `empty()` | $O(1)$ | $O(1)$ |
| `insert` / `erase` / `set` | 償却 $O(\log N)$ | $O(1)$ |
| `maximum_sum` | 償却 $O(\log N)$ | $O(1)$ |

## 注意点

- 同時に保持する要素数は `MAX_SIZE` 以下でなければなりません。
- `maximum_sum` は非空区間だけを受け付けます。
- 添字・容量違反では `runtime_error` を送出します。
