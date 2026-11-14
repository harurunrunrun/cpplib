---
title: Maximum Subarray Sum Segment Tree (最大部分配列和セグメント木) [GSS1] [GSS3]
documentation_of: ../../../../src/structure/segtree/specialized/maximum_subarray_sum_segtree.hpp
---

1 点代入に対応し、指定した区間に含まれる非空部分配列の最大和を返すセグメント木。

## API

### `MaximumSubarraySumSegtree<T, MAX_SIZE>(values)`

初期列から構築する。`MAX_SIZE` を省略すると可変長版の `Segtree` を内部で使い、長さの上限がなくなる。

### `size()`

要素数を返す。

### `set(position, value)`

1 点を `value` に代入する。

### `maximum_sum(left, right)`

非空の半開区間 $[left, right)$ に含まれる非空部分配列の最大和を返す。

## API 別の時間計算量・空間計算量

$N$ を要素数とする。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(\texttt{MAX_SIZE})$ |
| `size()` | $O(1)$ | $O(1)$ |
| `set(...)` | $O(\log N)$ | $O(1)$ |
| `maximum_sum(...)` | $O(\log N)$ | $O(1)$ |

## 注意点

- 固定容量版では $N \le \texttt{MAX_SIZE}$ が必要である。
- `maximum_sum` は非空区間だけを受け付け、`left >= right` では `std::runtime_error` を送出する。
- その他の範囲違反でも（内部の `Segtree` から）`std::runtime_error` を送出する。
- 和が `T` で表現できなければならない。
