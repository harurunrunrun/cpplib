---
title: Dynamic Maximum Subarray Sum Sequence (動的列最大部分配列和) [GSS6]
documentation_of: ../../../../src/structure/bbst/sequence/dynamic_maximum_subarray_sum_sequence.hpp
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

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = DynamicMaximumSubarraySumSequence<T>;
using Fixed = DynamicMaximumSubarraySumSequence<T, N>;
```

可変長版は実行時の要素数・頂点数に応じて配列を確保する。ノードを追加する構造では伸長費用が償却 $O(1)$、既存の操作計算量を維持する。
領域は使用ノード数に比例し、木の距離索引など既存の段別領域は上記と同じ漸近量。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
