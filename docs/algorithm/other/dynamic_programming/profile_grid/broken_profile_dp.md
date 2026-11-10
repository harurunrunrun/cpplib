---
title: Broken Profile DP (輪郭線DP)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/profile_grid/broken_profile_dp.hpp
---

格子を行優先（row-major）順に 1 マスずつ走査し、幅方向のビットマスクだけを状態として保持する broken profile DP。名前空間は `dynamic_programming` である。

## `broken_profile_dp<Count>(row_count, column_count, initial_mask, transition)`

各マス・各到達済みマスクについて `transition(row, column, mask, emit)` を呼ぶ。`emit(next_mask, multiplier)` を呼ぶと、次のマスの状態 `next_mask` に現在の場合の数の `multiplier` 倍が加算される。全マスを処理した後の、マスクごとの場合の数を `std::vector<Count>`（長さ $2^W$）で返す。

幅を $W$、高さを $H$、1 状態からの遷移数の最大値を $D$ とすると、時間計算量は $O(HW2^W (D+1))$、空間計算量は $O(2^W)$ である。`Count` は `Count{}` が $0$、`Count{1}` が $1$ を表し、`+=`、`*`、`==` を提供する必要がある。

## `count_domino_tilings_broken_profile<Count>(row_count, column_count)`

$H \times W$ の長方形を $1\times2$ のドミノで敷き詰める方法の数を返す（`Count` のデフォルトは `std::uint64_t` で、オーバーフローは検査しない）。短い辺を幅に選ぶ。時間計算量 $O(HW2^{\min(H,W)})$、空間計算量 $O(2^{\min(H,W)})$。

## 注意点

- `transition` は、現在のマスを処理済みにした後のマスクを出力する。
- 幅は `std::size_t` のビット数未満でなければならず、違反時や `initial_mask` が $2^W$ 以上のときは `std::invalid_argument` を送出する。`emit` に $2^W$ 以上のマスクを渡すと `std::out_of_range` を送出する。
