---
title: Range Assign Add Sum of Squares (区間代入・加算・二乗和) [SEGSQRSS]
documentation_of: ../../../../src/structure/segtree/specialized/range_assign_add_sum_squares.hpp
---

区間代入と区間加算に対応し、区間内要素の二乗和を返します。

## API

### `RangeAssignAddSumSquares(values)`

初期列から構築します。

### `size()`

要素数を返します。

### `assign(left, right, value)`

半開区間 `[left,right)` の全要素を `value` に代入します。

### `add(left, right, value)`

半開区間の全要素へ `value` を加えます。

### `square_sum(left, right)`

半開区間の $\sum a_i^2$ を返します。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N)$ | $O(MAX\_SIZE)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `assign` / `add` / `square_sum` | $O(\log N)$ | $O(1)$ |

## 注意点

- `N <= MAX_SIZE` が必要です。
- 区間は半開区間です。範囲違反では `runtime_error` を送出します。
- 和、二乗和、作用途中の積が `T` で表現できなければなりません。

## 容量を省略する場合

容量引数の既定値は `-1`。`RangeAssignAddSumSquares<T> tree(values);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
