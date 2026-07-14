---
title: Static Two-Ranges Maximum Subarray Sum (二区間制約付き静的最大部分配列和) [GSS5]
documentation_of: ../src/structure/segtree/static_two_ranges_maximum_subarray_sum.hpp
---

始点を第一の区間、終点を第二の区間から選ぶ非空部分配列の最大和を返す静的構造です。

## API

### `StaticTwoRangesMaximumSubarraySum(values)`

初期列を構築します。

### `size()`

要素数を返します。

### `maximum_sum(first_left, first_right, second_left, second_right)`

始点が `[first_left, first_right)`、終点が `[second_left, second_right)` に属し、始点が終点以下となる部分配列の最大和を返します。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N)$ | $O(MAX\_SIZE)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `maximum_sum(...)` | $O(\log N)$ | $O(1)$ |

## 注意点

- 両区間は非空で、`0 <= first_left <= second_left < second_right <= N` および `first_left < first_right <= second_right` が必要です。
- `N <= MAX_SIZE` が必要です。
- 不正な区間では `runtime_error` を送出します。
- 部分配列和が `T` で表現できる必要があります。
