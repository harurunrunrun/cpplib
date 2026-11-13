---
title: Range Integer Square Root Sum (区間整数平方根・区間和) [GSS4]
documentation_of: ../../../../src/structure/segtree/specialized/range_integer_sqrt_sum.hpp
---

区間内の各値を整数平方根に置き換える操作と、区間和の取得を行う列。

## API

### `RangeIntegerSqrtSum<MAX_SIZE>(values)`

`std::vector<long long>` の初期列から構築する。内部では和の `Segtree` と、まだ 2 以上の値を持つ位置の集合（`std::set`）を使う。`MAX_SIZE` を省略すると可変長版になり、長さの上限がなくなる。

### `size()`

要素数を返す。

### `apply_sqrt(left, right)`

半開区間 $[left, right)$ の各値 $x$ を $\lfloor\sqrt{x}\rfloor$ に置き換える。

### `sum(left, right)`

半開区間 $[left, right)$ の和を返す。

## API 別の時間計算量・空間計算量

$N$ を要素数とし、`apply_sqrt` で実際に調べる不安定な（2 以上の）要素数を $K$ とする。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N\log N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `apply_sqrt(l, r)` | $O((K+1)\log N)$ | $O(1)$ |
| `sum(l, r)` | $O(\log N)$ | $O(1)$ |

各要素が不安定な要素として更新される回数は、初期値を $A$ として $O(\log\log(A+2))$ 回である。

## 注意点

- 固定容量版では $N \le \texttt{MAX_SIZE}$ が必要である。
- 値は非負を想定する。値が 0 または 1 になると、以降の平方根の更新を省略する（1 以下の値は最初から更新しない）。
- 区間は半開区間である。`apply_sqrt` の範囲違反では `std::runtime_error` を送出し、`sum` の範囲違反では内部の `Segtree` が `std::runtime_error` を送出する。
