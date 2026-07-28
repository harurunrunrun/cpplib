---
title: Digit DP Framework (桁DPフレームワーク)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/digit/digit_dp.hpp
---

上限の各桁を左から処理し、`tight`・先頭非零桁の開始状態・利用者定義状態をまとめて数える。

## `digit_dp_count<Count>(upper_digits, base, initial_state, transition, accept, hash, equal)`

`transition(state, digit, started, next_started, position)` は遷移不能なら `nullopt`、可能なら次状態を返す。`accept(state, started)` が最終状態を採用するか決める。桁数を $L$、到達状態数を $S$、基数を $B$ とすると時間計算量 $O(LSB)$、空間計算量 $O(S)$。

## `digit_dp_count_range<Count>(lower_digits, upper_digits, ...)`

閉区間 `[lower, upper]` の個数を、上限以下の個数の差として返す。時間計算量 $O(LSB)$、空間計算量 $O(S)$。`Count` は減算を提供する必要がある。

## `decimal_digits(value)`

10進文字列を桁vectorへ変換する。時間・空間計算量は $O(L)$。

## 注意点

`upper_digits` は最上位桁から並べる。先頭0も `transition` に渡されるため、数0を含める規則を明示できる。不正な基数・桁・範囲には `std::invalid_argument` を送出する。
