---
title: First-Fit Bin Packing Summary (First-Fitビン詰め集計) [HELPR2D2]
documentation_of: ../src/approximate/packing/first_fit_bin_packing_summary.hpp
---

First-Fit法でビン詰めした結果の使用ビン数と未使用容量の総和を返す。

# FirstFitBinPackingSummary

```cpp
struct FirstFitBinPackingSummary {
    size_t used_bin_count;
    long long unused_capacity;
};
```

# first_fit_bin_packing_summary

```cpp
FirstFitBinPackingSummary first_fit_bin_packing_summary(
    const vector<int>& item_size,
    int capacity
);
```

入力順にFirst-Fit法を適用し、集計結果を返す。

## 時間計算量

品物数を $N$ として $O(N\log N)$。

## 空間計算量

$O(N)$。

## 注意点

- `capacity` は正で、各品物の大きさは $[0,capacity]$ でなければならない。
- First-Fit法は近似法であり、使用ビン数の最適性を保証しない。
- 前提違反時の扱いは `first_fit_bin_packing` と同じである。
