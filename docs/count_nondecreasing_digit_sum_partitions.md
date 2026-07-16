---
title: Nondecreasing Digit-Sum Partitions (桁和非減少分割の個数) [ANARC05H]
documentation_of: ../src/algorithm/string/count_nondecreasing_digit_sum_partitions.hpp
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
| `count_nondecreasing_digit_sum_partitions` | $O(N^3)$ | $O(N^2)$ |

## 注意点

- 数字以外を含む場合は `runtime_error`、答えが型を超える場合は `overflow_error` となる。
