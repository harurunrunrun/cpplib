---
title: Maximum Rental Profit (互いに重ならない貸出の最大利益) [RENT]
documentation_of: ../src/algorithm/other/maximum_rental_profit.hpp
---

時間区間付き依頼から互いに重ならない依頼を選ぶ最大利益を返す。

# RentalOffer

```cpp
struct RentalOffer { long long start, duration, profit; };
```

開始時刻、正の継続時間、非負の利益を保持する。

# maximum_rental_profit

```cpp
long long maximum_rental_profit(vector<RentalOffer> offers);
```

終了時刻と別依頼の開始時刻が等しい組合せを許す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_rental_profit` | $O(N\log N)$ | $O(N)$ |

## 注意点

- 不正な継続時間または利益では `runtime_error` を送出する。
