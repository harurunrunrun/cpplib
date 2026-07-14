---
title: Prefix Sum (累積和) [static_range_sum]
documentation_of: ../src/structure/other/prefix_sum.hpp
---

静的な一次元列・二次元行列の区間和を返す累積和。
区間は半開区間で指定する。値型 `T` はデフォルト構築、加算、減算を使える必要がある。

## PrefixSum

```cpp
PrefixSum<T, MAX_SIZE> sum(values);
sum.prefix_sum(r);
sum.sum(l, r);
sum.get(i);
```

| 操作 | 時間計算量 |
| --- | --- |
| デフォルト構築、`PrefixSum(n)` | $O(MAX\_SIZE)$ |
| `PrefixSum(values)` | $O(MAX\_SIZE+n)$ |
| `build(values)` | $O(n)$ |
| `size()`, `empty()` | $O(1)$ |
| `prefix_sum(r)` | $O(1)$ |
| `sum(l, r)` | $O(1)$ |
| `get(i)` | $O(1)$ |

保存領域は $O(MAX\_SIZE)$。

## PrefixSum2D

```cpp
PrefixSum2D<T, MAX_HEIGHT, MAX_WIDTH> sum(matrix);
sum.sum(row_begin, col_begin, row_end, col_end);
sum.get(row, col);
```

`sum` は `[row_begin, row_end) x [col_begin, col_end)` の和を返す。
$C=(MAX\_HEIGHT+1)(MAX\_WIDTH+1)$、入力の大きさを $H\times W$ とする。

| 操作 | 時間計算量 |
| --- | --- |
| デフォルト構築、`PrefixSum2D(height, width)` | $O(C)$ |
| `PrefixSum2D(matrix)` | $O(C+HW)$ |
| `build(matrix)` | $O(HW)$ |
| `height()`, `width()`, `empty()` | $O(1)$ |
| `sum(row_begin, col_begin, row_end, col_end)` | $O(1)$ |
| `get(row, col)` | $O(1)$ |

保存領域は $O(C)$。

## 空間計算量（API別の追加空間計算量）

- `PrefixSum`の全constructor、`build`、`size`、`empty`、`prefix_sum`、`sum`、`get`: $O(1)$
- `PrefixSum2D`の全constructor、`build`、`height`、`width`、`empty`、`sum`、`get`: $O(1)$

戻り値を除く一時領域を数えている。構築結果の固定保存領域は1次元が $O(\mathtt{MAX\_SIZE})$、2次元が $O(C)$ である。

## 注意点

- 1次元は`0 <= n <= MAX_SIZE`。index、prefix端点、半開区間の範囲違反は例外。
- 2次元は`0 <= H <= MAX_HEIGHT`, `0 <= W <= MAX_WIDTH`。非矩形入力も例外。
- 2次元矩形は`0 <= row_begin <= row_end <= H`かつ
  `0 <= col_begin <= col_end <= W`。点indexも実際のshape内でなければならない。
- default constructorの実際の長さ・shapeは0。`build`で同じobjectを再構築できる。
- 容量はtemplate引数で固定され、構築後の値更新APIはない。
