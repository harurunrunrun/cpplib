---
title: Digit DP Framework (桁DPフレームワーク)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/digit/digit_dp.hpp
---

上限の各桁を最上位から順に処理し、上限に一致しているか（`tight`）、先頭の非零桁が始まったか（`started`）、利用者が定義した状態の 3 つをまとめて数え上げる。状態は `std::unordered_map` で管理する。名前空間は `dynamic_programming` である。

## `digit_dp_count<Count>(upper_digits, base, initial_state, transition, accept, hash, equal)`

$0$ 以上 `upper_digits` が表す値以下の整数（`upper_digits` と同じ桁数に先頭を $0$ で埋めた桁列）のうち、条件を満たすものの個数を返す。

- `transition(state, digit, started, next_started, position)` は、遷移できなければ `std::nullopt` を、できれば次の状態を返す。`position` は最上位桁を $0$ とする桁の位置である。
- `accept(state, started)` は、最終状態を数えるかどうかを返す。
- `hash`, `equal` は状態のハッシュ関数と等値比較で、デフォルトは `std::hash<State>`, `std::equal_to<State>` である。
- `Count` はデフォルト構築による $0$、`Count{1}` の構築、`+=` を提供する必要がある。

桁数を $L$、各桁で到達する状態数を $S$、基数を $B$ とすると、時間計算量は期待 $O(LSB)$、空間計算量は $O(S)$ である。

## `digit_dp_count_range<Count>(lower_digits, upper_digits, base, initial_state, transition, accept, hash, equal)`

閉区間 $[\mathrm{lower}, \mathrm{upper}]$ に含まれる個数を、`upper` 以下の個数と `lower - 1` 以下の個数の差として返す（`lower` が $0$ なら `upper` 以下の個数をそのまま返す）。`lower_digits` と `upper_digits` は同じ桁数でなければならない。時間計算量は $O(LSB)$、空間計算量は $O(S)$ である。`Count` は減算も提供する必要がある。

## `decimal_digits(value)`

十進表記の文字列を桁の `std::vector<int>` に変換する。時間・空間計算量は $O(L)$ である。

## 注意点

- 桁列は最上位桁から並べる。先頭の $0$ も `transition` に渡されるため、数 $0$ を含めるかどうかの規則を `accept` で明示できる。
- 基数が $2$ 未満、桁列が空、桁が基数の範囲外、区間の桁数の不一致や `lower > upper`、`decimal_digits` への空文字列や数字以外の文字には `std::invalid_argument` を送出する。
