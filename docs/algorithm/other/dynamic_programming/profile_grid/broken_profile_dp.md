---
title: Broken Profile DP (輪郭線DP)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/profile_grid/broken_profile_dp.hpp
---

格子をrow-major順に走査し、幅方向のbitmaskだけを保持するbroken-profile DP。

## `broken_profile_dp<Count>(row_count, column_count, initial_mask, transition)`

各cell・各到達maskについて `transition(row, column, mask, emit)` を呼ぶ。`emit(next_mask, multiplier)` で次のcellへ遷移する。幅を $W$、高さを $H$、1状態からの最大遷移数を $D$ とすると時間計算量 $O(HW2^W D)$、空間計算量 $O(2^W)$。

## `count_domino_tilings_broken_profile<Count>(row_count, column_count)`

長方形を $1\times2$ dominoで敷き詰める方法数。短辺を幅に選ぶ。時間計算量 $O(HW2^{\min(H,W)})$、空間計算量 $O(2^{\min(H,W)})$。

## 注意点

`transition` は現在cellを処理済みにしたmaskを出力する。幅は `size_t` のbit数未満でなければならない。不正maskには `std::invalid_argument` または `std::out_of_range` を送出する。
