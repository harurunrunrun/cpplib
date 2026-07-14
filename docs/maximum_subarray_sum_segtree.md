---
title: Maximum Subarray Sum Segment Tree (最大部分配列和セグメント木) [GSS1] [GSS3]
documentation_of: ../src/structure/segtree/maximum_subarray_sum_segtree.hpp
---

一点代入に対応し、非空区間の最大部分配列和を返すセグメント木です。

## API

### `MaximumSubarraySumSegtree(values)`

初期列から構築します。

### `size()`

要素数を返します。

### `set(position, value)`

一点を `value` に代入します。

### `maximum_sum(left, right)`

非空半開区間 `[left,right)` に含まれる非空部分配列の最大和を返します。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N)$ | $O(MAX\_SIZE)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `set(...)` | $O(\log N)$ | $O(1)$ |
| `maximum_sum(...)` | $O(\log N)$ | $O(1)$ |

## 注意点

- `N <= MAX_SIZE` が必要です。
- `maximum_sum` は非空区間だけを受け付けます。
- 範囲違反では `runtime_error` を送出します。
- 和が `T` で表現できなければなりません。
