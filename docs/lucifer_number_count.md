---
title: Lucifer Number Count (Lucifer数の区間個数) [LUCIFER]
documentation_of: ../src/algorithm/math/lucifer_number_count.hpp
---

十進桁の偶数位置和と奇数位置和の差が素数になる整数を閉区間で数える。

# `lucifer_number_count`

```cpp
uint64_t lucifer_number_count(uint64_t left, uint64_t right);
```

位置は最下位桁を1番目として数え、`even-position sum - odd-position sum` が正の素数となる整数の個数を返す。

## API別の時間計算量・空間計算量

桁数を `D` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `lucifer_number_count` | $O(10D^2)$ | $O(D^2)$ |

## 注意点

- `left <= right` が必要である。
