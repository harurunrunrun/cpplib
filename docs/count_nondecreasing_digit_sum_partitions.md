---
title: Nondecreasing Digit-Sum Partitions (桁和非減少分割の個数) [ANARC05H]
documentation_of: ../src/algorithm/string/sequence/count_nondecreasing_digit_sum_partitions.hpp
---

数字列を各部分の桁和が非減少になるよう分割する方法数を返す。

## count_nondecreasing_digit_sum_partitions

```cpp
unsigned long long count_nondecreasing_digit_sum_partitions(const string& digits);
```

空でない連続部分列への全分割を数える。空文字列では1を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_nondecreasing_digit_sum_partitions` | $O(N^2)$ | $O(N^2)$ |

各位置を次の部分列の始点として走査する。直前の部分列の桁和ごとの
DPを累積和に変換しておくことで、次の桁和以下に対応する遷移を
$O(1)$ で取得する。すべての終点を列挙する時間と、各DP行の累積和を
作る時間はいずれも合計 $O(N^2)$ である。

## 注意点

- 数字以外を含む場合は `runtime_error` となる。
- 最終的な答えが `unsigned long long` の範囲を超える場合は `overflow_error` となる。
- 桁和を `int` で表現できない長さの入力では `length_error` となる。
