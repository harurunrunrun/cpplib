---
title: Floor Sum (床関数和)
documentation_of: ../src/algorithm/math/floor_sum.hpp
---

## 関数

```cpp
long long floor_sum(long long n, long long m, long long a, long long b)
```

$$
\sum_{i=0}^{n-1} \left\lfloor \frac{ai+b}{m} \right\rfloor
$$

を返す。`n >= 0`, `m > 0` でなければならない。`a`, `b` は負でもよい。
答えが `long long` に収まらない場合は `std::overflow_error` を送出する。

## 計算量の概要

$O(\log m)$

## API別の時間計算量・空間計算量

| API | 時間計算量 | 補足 |
| --- | --- | --- |
| `math::floor_sum(n, m, a, b)` | $O(\log m)$ | Euclid 型の反復。追加領域は $O(1)$。 |

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
