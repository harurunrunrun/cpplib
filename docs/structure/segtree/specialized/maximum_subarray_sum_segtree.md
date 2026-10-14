---
title: Maximum Subarray Sum Segment Tree (最大部分配列和セグメント木) [GSS1] [GSS3]
documentation_of: ../../../../src/structure/segtree/specialized/maximum_subarray_sum_segtree.hpp
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

## 容量を省略する場合

容量引数の既定値は `-1`。`MaximumSubarraySumSegtree<T> tree(values);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
