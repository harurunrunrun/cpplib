---
title: Square Divisor Count Prefix Sum (平方数の約数個数の累積和) [DIVCNT2]
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/square_divisor_count_prefix_sum.hpp
---

各正整数の平方 $i^2$ の約数の個数 $\tau(i^2)$ について、累積和 $\sum_{i=1}^{n} \tau(i^2)$ を複数回求める。

$\tau(i^2) = \sum_{d \mid i} \lvert \mu(d) \rvert \, \tau(i/d)$ を用い、商 $\lfloor n/d \rfloor$ ごとにまとめて、無平方数の個数の累積和と約数の個数の累積和の積を足し合わせる。小さい引数は前計算表を引き、大きい引数はその場で $O(\sqrt x)$ 時間で計算する。

## `SquareDivisorCountPrefixSum`

```cpp
explicit math::SquareDivisorCountPrefixSum(std::uint64_t maximum_query);
```

$0 \le n \le N$（$N$ は `maximum_query`）の範囲のクエリに必要な前計算を行う。`maximum_query` は以後の `query` で指定する $n$ の最大値である。

## `query`

```cpp
std::uint64_t query(std::uint64_t n) const;
```

$\sum_{i=1}^{n} \tau(i^2)$、すなわち各 $i^2$ の正の約数の個数の合計を返す。$n$ は累積和の右端で、$n = 0$ では $0$ を返す。

## `query_all`

```cpp
std::vector<std::uint64_t> query_all(const std::vector<std::uint64_t>& queries) const;
```

`queries` の各要素（累積和の右端）について `query` した結果を、入力と同じ順に返す。

## `square_divisor_count_prefix_sums`

```cpp
std::vector<std::uint64_t> math::square_divisor_count_prefix_sums(
    const std::vector<std::uint64_t>& queries);
```

`queries` の最大値に合わせて前計算を一度だけ行い、すべてのクエリの答えを入力と同じ順に返す。

## 時間計算量・空間計算量

$N$ を `maximum_query`、$Q$ をクエリの個数とする。前計算の幅 $B$ は $\min\left(N+1, \max\left(\lfloor \sqrt N \rfloor + 1, \min(\lceil \sqrt[3]{N} \rceil^2 + 1, 1.6 \times 10^7)\right)\right)$（$N < 2$ では $2$）に設定される。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `SquareDivisorCountPrefixSum` | $O(B)$ | $O(B)$ |
| `query` | $O(N/\sqrt B + \sqrt N)$ | $O(1)$ |
| `query_all` | 各クエリの `query` の和 | 戻り値を除き $O(1)$ |
| `square_divisor_count_prefix_sums` | $O(B + \sum (n/\sqrt B + \sqrt n))$ | $O(B + Q)$ |

## 注意点

- `maximum_query` が $10^{12}$ 以下でなければならず、範囲外では `std::invalid_argument` を送出する。
- `query` の引数が構築時の `maximum_query` を超えると `std::out_of_range` を送出する。
- 答えが `uint64_t` に収まらない場合は `std::overflow_error` を送出する。
