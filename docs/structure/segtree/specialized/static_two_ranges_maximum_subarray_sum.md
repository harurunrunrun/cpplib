---
title: Static Two-Ranges Maximum Subarray Sum (二区間制約付き静的最大部分配列和) [GSS5]
documentation_of: ../../../../src/structure/segtree/specialized/static_two_ranges_maximum_subarray_sum.hpp
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

## 容量を省略する場合

容量引数の既定値は `-1`。`StaticTwoRangesMaximumSubarraySum<T> tree(values);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
