---
title: Static Two-Ranges Maximum Subarray Sum (二区間制約付き静的最大部分配列和) [GSS5]
documentation_of: ../../../../src/structure/segtree/specialized/static_two_ranges_maximum_subarray_sum.hpp
---

始点を第一の区間から、終点を第二の区間から選ぶ非空部分配列の最大和を返す静的なデータ構造。

## API

### `StaticTwoRangesMaximumSubarraySum<T, MAX_SIZE>(values)`

初期列から構築する。内部では `Segtree` を使い、`MAX_SIZE` を省略すると可変長版になって長さの上限がなくなる。

### `size()`

要素数を返す。

### `maximum_sum(first_left, first_right, second_left, second_right)`

始点が `[first_left, first_right)`、終点が `[second_left, second_right)` に属し、始点が終点以下となる部分配列の和の最大値を返す。

## API 別の時間計算量・空間計算量

$N$ を要素数とする。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(\texttt{MAX_SIZE})$ |
| `size()` | $O(1)$ | $O(1)$ |
| `maximum_sum(...)` | $O(\log N)$ | $O(1)$ |

## 注意点

- 両区間は非空で、`0 <= first_left <= second_left < second_right <= N` および `first_left < first_right <= second_right` が必要である。
- 固定容量版では $N \le \texttt{MAX_SIZE}$ が必要である。
- 不正な区間では `std::runtime_error` を送出する。
- 部分配列の和が `T` で表現できる必要がある。
