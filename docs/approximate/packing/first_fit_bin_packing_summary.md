---
title: First-Fit Bin Packing Summary (First-Fit ビン詰め集計) [HELPR2D2]
documentation_of: ../../../src/approximate/packing/first_fit_bin_packing_summary.hpp
---

First-Fit 法でビン詰めした結果の使用ビン数と、未使用容量の総和を返す。

## FirstFitBinPackingSummary

```cpp
struct FirstFitBinPackingSummary {
    size_t used_bin_count;
    long long unused_capacity;
};
```

## first_fit_bin_packing_summary

```cpp
FirstFitBinPackingSummary first_fit_bin_packing_summary(
    const vector<int>& item_size,
    int capacity
);
```

入力順に First-Fit 法（`first_fit_bin_packing`）を適用し、集計結果を返す。

## 時間計算量

品物数を $N$ として $O(N \log N)$。

## 空間計算量

$O(N)$。

## 注意点

- `capacity` は正で、各品物の大きさは $[0, \mathrm{capacity}]$ に入っていなければならない。前提違反時は `first_fit_bin_packing` と同様に `std::invalid_argument` を送出する。
- First-Fit 法は近似解法であり、使用ビン数の最適性は保証しない。
