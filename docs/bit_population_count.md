---
title: Bit Population Count over Integer Ranges (整数区間のビット数え上げ)
documentation_of: ../src/algorithm/math/combinatorics/bit_population_count.hpp
---

非負整数の連続区間に現れる 1-bit の個数を、bit ごとの周期性から数える。
答えは最大で $64\cdot2^{63}$ になるため `unsigned __int128` の別名
`math::WideBitCount` を使う。

## `count_bit_ones_inclusive`

```cpp
math::WideBitCount count_bit_ones_inclusive(
    std::uint64_t maximum,
    unsigned bit
)
```

$0\le x\le maximum$ の各 $x$ について、`bit` 番目が 1 である個数を返す。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## `count_bit_ones_half_open`

```cpp
math::WideBitCount count_bit_ones_half_open(
    std::uint64_t right,
    unsigned bit
)
```

$0\le x<right$ の各 $x$ について、`bit` 番目が 1 である個数を返す。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## `bit_one_counts_inclusive`

```cpp
std::array<math::WideBitCount, 64>
bit_one_counts_inclusive(std::uint64_t maximum)
```

$[0,maximum]$ における各 bit の個数をまとめて返す。配列の添字が bit 番号。

- 時間計算量: $O(64)$
- 追加空間計算量: 戻り値 $O(64)$

## `bit_one_counts_half_open`

```cpp
std::array<math::WideBitCount, 64>
bit_one_counts_half_open(std::uint64_t right)
```

$[0,right)$ における各 bit の個数をまとめて返す。

- 時間計算量: $O(64)$
- 追加空間計算量: 戻り値 $O(64)$

## `total_set_bits_inclusive`

```cpp
math::WideBitCount total_set_bits_inclusive(std::uint64_t maximum)
```

$[0,maximum]$ の全整数の `popcount` の和を返す。

- 時間計算量: $O(64)$
- 追加空間計算量: $O(64)$

## `total_set_bits_half_open`

```cpp
math::WideBitCount total_set_bits_half_open(std::uint64_t right)
```

$[0,right)$ の全整数の `popcount` の和を返す。

- 時間計算量: $O(64)$
- 追加空間計算量: $O(64)$

## 注意点

bit 番号は $0\le bit<64$。違反時は `runtime_error`。`WideBitCount` の入出力は
標準streamに直接対応していないため、必要なら10進文字列への変換を用意する。

