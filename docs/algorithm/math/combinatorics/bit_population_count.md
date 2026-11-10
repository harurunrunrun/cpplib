---
title: Bit Population Count over Integer Ranges (整数区間のビット数え上げ)
documentation_of: ../../../../src/algorithm/math/combinatorics/bit_population_count.hpp
---

非負整数の連続区間に現れる 1 のビットの個数を、ビットごとの周期性を利用して数える。答えは最大で $64 \cdot 2^{63}$ になるため、`unsigned __int128` の別名 `math::WideBitCount` を使う。

## `count_bit_ones_inclusive`

```cpp
math::WideBitCount count_bit_ones_inclusive(
    std::uint64_t maximum,
    unsigned bit
)
```

`maximum` を $m$、`bit` を $b$ とする。$0 \le x \le m$ を満たす $x$ のうち、第 $b$ ビットが 1 であるものの個数を返す。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## `count_bit_ones_half_open`

```cpp
math::WideBitCount count_bit_ones_half_open(
    std::uint64_t right,
    unsigned bit
)
```

`right` を $r$ とする。$0 \le x < r$ を満たす $x$ のうち、第 $b$ ビットが 1 であるものの個数を返す。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## `bit_one_counts_inclusive`

```cpp
std::array<math::WideBitCount, 64>
bit_one_counts_inclusive(std::uint64_t maximum)
```

区間 $[0, m]$ における各ビットの 1 の個数をまとめて返す。配列の添字がビット番号である。

- 時間計算量: $O(64)$
- 追加空間計算量: 戻り値の $O(64)$

## `bit_one_counts_half_open`

```cpp
std::array<math::WideBitCount, 64>
bit_one_counts_half_open(std::uint64_t right)
```

区間 $[0, r)$ における各ビットの 1 の個数をまとめて返す。

- 時間計算量: $O(64)$
- 追加空間計算量: 戻り値の $O(64)$

## `total_set_bits_inclusive`

```cpp
math::WideBitCount total_set_bits_inclusive(std::uint64_t maximum)
```

区間 $[0, m]$ の全整数の `popcount` の和を返す。

- 時間計算量: $O(64)$
- 追加空間計算量: $O(64)$

## `total_set_bits_half_open`

```cpp
math::WideBitCount total_set_bits_half_open(std::uint64_t right)
```

区間 $[0, r)$ の全整数の `popcount` の和を返す。

- 時間計算量: $O(64)$
- 追加空間計算量: $O(64)$

## 注意点

ビット番号は $0 \le b < 64$ でなければならず、違反時は `runtime_error` を送出する。いずれの関数も `constexpr` である。`WideBitCount` は標準ストリームでの入出力に直接対応していないため、必要なら 10 進文字列への変換を用意する。
