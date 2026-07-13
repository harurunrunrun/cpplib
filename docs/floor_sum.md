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

## 計算量

$O(\log m)$

## API別計算量

| API | 時間計算量 | 補足 |
| --- | --- | --- |
| `math::floor_sum(n, m, a, b)` | $O(\log m)$ | Euclid 型の反復。追加領域は $O(1)$。 |
