---
title: Prefix Sum (累積和) [static_range_sum]
documentation_of: ../../../../src/structure/range_query/aggregation/prefix_sum.hpp
---

静的な 1 次元の列・2 次元の行列の区間和を返す累積和である。区間は半開区間で指定する。値型 `T` はデフォルト構築・加算・減算ができる必要がある。

```cpp
template<class T, int MAX_SIZE = -1>
struct PrefixSum;

template<class T, int MAX_HEIGHT = -1, int MAX_WIDTH = -1>
struct PrefixSum2D;
```

容量のテンプレート引数は長さ・行数・列数の上限である。省略時（`-1`）は上限を設けず、実行時の大きさに合わせて `std::vector` で領域を確保する。

## PrefixSum

```cpp
PrefixSum<T, MAX_SIZE> sum(values);
sum.prefix_sum(r);
sum.sum(l, r);
sum.get(i);
```

`prefix_sum(r)` は `[0, r)` の和、`sum(l, r)` は `[l, r)` の和、`get(i)` は `i` 番目の要素を返す。

| 操作 | 時間計算量 |
| --- | --- |
| デフォルト構築、`PrefixSum(n)` | $O(\texttt{MAX_SIZE})$ |
| `PrefixSum(values)` | $O(\texttt{MAX_SIZE}+n)$ |
| `build(values)` | $O(n)$ |
| `size()`, `empty()` | $O(1)$ |
| `prefix_sum(r)` | $O(1)$ |
| `sum(l, r)` | $O(1)$ |
| `get(i)` | $O(1)$ |

保存領域は $O(\texttt{MAX_SIZE})$。容量を省略した場合、構築と保存領域は $O(n)$ である。

## PrefixSum2D

```cpp
PrefixSum2D<T, MAX_HEIGHT, MAX_WIDTH> sum(matrix);
sum.sum(row_begin, col_begin, row_end, col_end);
sum.get(row, col);
```

`sum` は `[row_begin, row_end) x [col_begin, col_end)` の和を返す。$C=(\texttt{MAX_HEIGHT}+1)(\texttt{MAX_WIDTH}+1)$ とし、入力の大きさを $H\times W$ とする。

| 操作 | 時間計算量 |
| --- | --- |
| デフォルト構築、`PrefixSum2D(height, width)` | $O(C)$ |
| `PrefixSum2D(matrix)` | $O(C+HW)$ |
| `build(matrix)` | $O(HW)$ |
| `height()`, `width()`, `empty()` | $O(1)$ |
| `sum(row_begin, col_begin, row_end, col_end)` | $O(1)$ |
| `get(row, col)` | $O(1)$ |

保存領域は $O(C)$。容量を省略した場合、構築と保存領域は $O(HW)$ である。

## 空間計算量（API 別の追加空間計算量）

- `PrefixSum` の全コンストラクタ、`build`、`size`、`empty`、`prefix_sum`、`sum`、`get`: $O(1)$
- `PrefixSum2D` の全コンストラクタ、`build`、`height`、`width`、`empty`、`sum`、`get`: $O(1)$

戻り値を除く一時領域を数えている。構築結果の保存領域は 1 次元が $O(\texttt{MAX_SIZE})$、2 次元が $O(C)$ である。

## 注意点

- 1 次元では `0 <= n <= MAX_SIZE` を要求する。添字、接頭辞の端点、半開区間が範囲外なら `std::runtime_error` を送出する。
- 2 次元では `0 <= H <= MAX_HEIGHT`, `0 <= W <= MAX_WIDTH` を要求する。行ごとに長さが異なる（矩形でない）入力も `std::runtime_error` を送出する。
- 2 次元の矩形は `0 <= row_begin <= row_end <= H` かつ `0 <= col_begin <= col_end <= W` を満たす必要がある。`get` の点の添字も実際の大きさの範囲内でなければならない。
- デフォルトコンストラクタで作ったオブジェクトの長さ・大きさは 0 である。`build` で同じオブジェクトを再構築できる。
- 構築後に値を更新する API はない。
