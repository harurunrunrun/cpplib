---
title: Nondecreasing Digit-Sum Partitions (桁和非減少分割の個数) [ANARC05H]
documentation_of: ../../../../src/algorithm/string/sequence/count_nondecreasing_digit_sum_partitions.hpp
---

数字列を、各部分の桁和が左から非減少になるように分割する方法の数を返す。

## count_nondecreasing_digit_sum_partitions

```cpp
unsigned long long count_nondecreasing_digit_sum_partitions(const string& digits);
```

空でない連続部分列へのすべての分割を数える。空文字列では 1 を返す。

## 時間計算量・空間計算量

$N$ を `digits` の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_nondecreasing_digit_sum_partitions` | $O(N^2)$ | $O(N^2)$ |

各位置を次の部分の始点として走査する。直前の部分の桁和ごとの
DP を累積和に変換しておくことで、次の部分の桁和以下に対応する遷移を
$O(1)$ で取得する。すべての終点を列挙する時間と、各 DP 行の累積和を
作る時間は、いずれも合計 $O(N^2)$ である。
DP 表は $(N+1)\times(9N+1)$ 個の 128 ビット整数なので、$N$ が数千を超えるとメモリ使用量に注意する。

## 注意点

- 数字以外を含む場合は `std::runtime_error` を送出する。
- 最終的な答えが `unsigned long long` の範囲を超える場合は `std::overflow_error` を送出する。
- 桁和を `int` で表せない長さの入力では `std::length_error` を送出する。
