---
title: Square Divisor Count Prefix Sum (平方数の約数個数の累積和) [DIVCNT2]
documentation_of: ../src/algorithm/math/square_divisor_count_prefix_sum.hpp
---

各正整数の平方が持つ約数の個数について、累積和を複数回求める。

## `SquareDivisorCountPrefixSum`

```cpp
explicit SquareDivisorCountPrefixSum(uint64_t maximum_query);
```

`0 <= n <= maximum_query` の問い合わせに必要な前計算を行う。

## 引数

- `maximum_query`: 以後の `query` で指定する最大値。

## `query`

```cpp
uint64_t query(uint64_t n) const;
```

`sum_{i=1}^n tau(i * i)` を返す。`n = 0` では `0` を返す。

## 引数

- `n`: 累積和の右端。

## 戻り値

各 `i^2` の正の約数の個数を合計した値。

## `query_all`

```cpp
vector<uint64_t> query_all(const vector<uint64_t>& queries) const;
```

各要素を `query` した結果を、入力と同じ順に返す。

## 引数

- `queries`: 累積和の右端の列。

## 戻り値

各問い合わせの答え。

## `square_divisor_count_prefix_sums`

```cpp
vector<uint64_t> square_divisor_count_prefix_sums(
    const vector<uint64_t>& queries);
```

`queries` の最大値に合わせて前計算を一度行い、全問い合わせの答えを返す。

## 引数

- `queries`: 累積和の右端の列。

## 戻り値

各問い合わせの答え。

## API別の時間計算量・空間計算量

`N = maximum_query` とする。前計算幅 `B` は
`max(floor(sqrt(N)) + 1, min(ceil(cuberoot(N))^2 + 1, 16000000))` 以下に調整される。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `SquareDivisorCountPrefixSum` | $O(B)$ | $O(B)$ |
| `query` | $O(N/\sqrt B+\sqrt N)$ | $O(1)$ |
| `query_all` | 各問い合わせの `query` の和 | 戻り値を除き $O(1)$ |
| `square_divisor_count_prefix_sums` | $O(B+\sum (n/\sqrt B+\sqrt n))$ | $O(B+Q)$ |

## 注意点

- `maximum_query <= 10^12` を前提とする。範囲外では `std::invalid_argument` を送出する。
- `query` の引数が構築時の `maximum_query` を超えると `std::out_of_range` を送出する。
- 答えが `uint64_t` に収まらない場合は `std::overflow_error` を送出する。
