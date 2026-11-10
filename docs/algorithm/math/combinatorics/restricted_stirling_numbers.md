---
title: Restricted Stirling Numbers (最小巡回長・最小ブロックサイズ付き Stirling 列)
documentation_of: ../../../../src/algorithm/math/combinatorics/restricted_stirling_numbers.hpp
---

巡回長・ブロックサイズに下限を課した Stirling 数の列（$k$ 固定、$n$ を動かした列）を求める。

## API

```cpp
template<int MOD>
std::vector<Modint<MOD>> math::restricted_stirling_numbers_first_kind_fixed_k(
    std::size_t maximum_order, std::size_t fixed_cycles, std::size_t minimum_cycle_size);

template<int MOD>
std::vector<Modint<MOD>> math::restricted_stirling_numbers_second_kind_fixed_k(
    std::size_t maximum_order, std::size_t fixed_blocks, std::size_t minimum_block_size);
```

以下、第 1 引数を $N$、第 2 引数を $K$、第 3 引数を $r$ とする。返り値の長さは常に $N + 1$ で、第 $n$ 要素（$n = 0, \ldots, N$）が次の値になる。

- 第一種（符号なし）: $n$ 個のラベル付き要素の順列のうち、ちょうど $K$ 個の巡回に分かれ、すべての巡回長が $r$ 以上であるものの個数。
- 第二種: $n$ 個のラベル付き要素をちょうど $K$ 個の空でないブロックに分ける集合分割のうち、すべてのブロックサイズが $r$ 以上であるものの個数。

$r = 1$ で通常の Stirling 数になり、$r = 2$ では固定点のない順列や、要素数 1 のブロックを持たない分割を数えることになる。

## 時間計算量

指数型母関数の係数列を形式的冪級数の冪で求める。母関数は第一種が $\frac{1}{K!}\left(\sum_{j \ge r} x^j / j\right)^K$、第二種が $\frac{1}{K!}\left(\sum_{j \ge r} x^j / j!\right)^K$ で、各係数に $n!$ を掛ける。多項式乗算の時間を $M(N)$ として $O(M(N))$ 時間、$O(N)$ 追加領域である。$K = 0$ または $Kr > N$ の場合は $O(N)$ 時間で返す。

## 注意点

- `MOD` は素数で、使用する NTT 長を法が許容する必要がある。
- $N \ge$ `MOD` なら `length_error`、$r = 0$ なら `invalid_argument` を送出する。
- $K = 0$ のときは空集合（$n = 0$）のみを 1 通りと数え、それ以外は $0$ とする。$Kr > N$ など実現不可能な条件では全要素 $0$ の列を返す。
