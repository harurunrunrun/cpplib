---
title: 0/1 Knapsack Heuristics and FPTAS (0/1ナップサックのヒューリスティックとFPTAS)
documentation_of: ../src/approximate/dp/knapsack.hpp
---

非負整数の重さ・価値を持つ 0/1 ナップサック問題に対する貪欲法と FPTAS。`approximate::dp` 名前空間にある。返値 `KnapsackResult` の `selected_indices` は選んだ元の添字の昇順、`total_weight` と `total_value` はその合計である。

# greedy_ratio_knapsack

~~~cpp
template<class Weight, class Value, class Capacity>
KnapsackResult greedy_ratio_knapsack(
    const vector<Weight>& weights,
    const vector<Value>& values,
    Capacity capacity
)
~~~

`value / weight` の降順に、入る品物を選ぶ。比は除算せず 128-bit 交差積で比較し、重さ 0 かつ価値が正の品物を先に選ぶ。同率なら添字が小さい品物を先に試す。単独では一般の近似保証を持たない。

- 時間計算量: $O(N\log N)$
- 追加空間計算量: $O(N)$

# greedy_value_knapsack

~~~cpp
template<class Weight, class Value, class Capacity>
KnapsackResult greedy_value_knapsack(
    const vector<Weight>& weights,
    const vector<Value>& values,
    Capacity capacity
)
~~~

価値の降順に、入る品物を選ぶ。同価値なら添字の昇順。一般の近似保証を持たない。

- 時間計算量: $O(N\log N)$
- 追加空間計算量: $O(N)$

# greedy_weight_knapsack

~~~cpp
template<class Weight, class Value, class Capacity>
KnapsackResult greedy_weight_knapsack(
    const vector<Weight>& weights,
    const vector<Value>& values,
    Capacity capacity
)
~~~

重さの昇順に、価値が正で入る品物を選ぶ。同じ重さなら添字の昇順。一般の近似保証を持たない。

- 時間計算量: $O(N\log N)$
- 追加空間計算量: $O(N)$

# modified_greedy_knapsack

~~~cpp
template<class Weight, class Value, class Capacity>
KnapsackResult modified_greedy_knapsack(
    const vector<Weight>& weights,
    const vector<Value>& values,
    Capacity capacity
)
~~~

比率順貪欲解と「重さ 0 の全品物 + 単独で入る最大価値の品物」を比較し、価値が大きい方を返す。同価値なら比率順貪欲解を返す。非負入力に対して最適値の $1/2$ 以上を保証する。

- 時間計算量: $O(N\log N)$
- 追加空間計算量: $O(N)$

# value_scaling_knapsack_fptas

~~~cpp
template<class Weight, class Value, class Capacity>
KnapsackResult value_scaling_knapsack_fptas(
    const vector<Weight>& weights,
    const vector<Value>& values,
    Capacity capacity,
    long double epsilon
)
~~~

$K=\varepsilon V_{max}/N'$ として正価値の候補品物の価値を `floor(value / K)` に丸め、丸めた価値ごとの最小重量 DP を行う。$K<1$ なら $K=1$ として不要な状態増加を避ける。重さ 0 の正価値品は常に選び、容量を単独で超える品物は候補から外す。`selected_indices` から近似解を復元する。

$0<\varepsilon<1$ に対して、返す価値は最適値の $(1-\varepsilon)$ 以上である。$N'$ を正価値かつ単独で容量内の正重量品数、$S$ を丸めた価値の総和とする。

- 時間計算量: $O(N'S)=O(N'^3/\varepsilon)$
- 追加空間計算量: $O(S+N'S/64)$ 個の機械語、すなわち $O(N'^2/\varepsilon+N'^3/(64\varepsilon))$

# KnapsackResult

`selected_indices`、`total_weight`、`total_value` はいずれも構築済みの値への参照または整数値なので、各フィールドの参照は時間・追加空間ともに $O(1)$。添字列の走査は $O(選択数)$。

# 前提・例外

`Weight`、`Value`、`Capacity` は `bool` 以外の 64-bit 以下の整数型とする。重さ・価値・容量は非負でなければならない。配列長の不一致、不正な入力、不正な `epsilon` には `std::invalid_argument` を送出する。復元表の添字を表現できない場合は `std::length_error`、返値の合計が `uint64_t` を超える場合は `std::overflow_error` を送出する。必要なメモリを確保できない場合は `std::bad_alloc` が送出され得る。
