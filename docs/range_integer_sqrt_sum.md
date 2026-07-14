---
title: Range Integer Square Root Sum (区間整数平方根・区間和) [GSS4]
documentation_of: ../src/structure/segtree/range_integer_sqrt_sum.hpp
---

区間内の各値を整数平方根へ置換し、区間和を取得する列です。

## API

### `RangeIntegerSqrtSum(values)`

初期列から構築します。

### `size()`

要素数を返します。

### `apply_sqrt(left, right)`

半開区間 `[left, right)` の各値 $x$ を $\lfloor\sqrt{x}\rfloor$ に置換します。

### `sum(left, right)`

半開区間 `[left, right)` の和を返します。

## API別の時間計算量・空間計算量

`apply_sqrt` で実際に調べる不安定要素数を $K$ とします。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N\log N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `apply_sqrt(l,r)` | $O((K+1)\log N)$ | $O(1)$ |
| `sum(l,r)` | $O(\log N)$ | $O(1)$ |

各非負要素が不安定要素として更新される回数は、初期値を $A$ として $O(\log\log(A+2))$ 回です。

## 注意点

- `N <= MAX_SIZE` が必要です。
- 想定する値は非負です。値が $0$ または $1$ になると以降の平方根更新を省略します。
- 区間は半開区間で、範囲違反では基盤構造が `runtime_error` を送出します。
