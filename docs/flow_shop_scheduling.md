---
title: Flow Shop Scheduling
documentation_of: ../src/approximate/scheduling/flow_shop.hpp
---

`processing_time[job][machine]`で表すpermutation flow shopを扱う。

## `flow_shop_makespan`

```cpp
Time value = flow_shop_makespan(processing_time, order);
```

重複のないjob列`order`を全機械で同じ順に処理したmakespanを返す。
部分列も受け付ける。job数を$N$、列長を$K$、機械数を$M$とすると、
入力行列の検査を含めて時間$O(NM+KM)$、追加領域$O(N+M)$。

## `johnson_two_machine_order`

```cpp
auto order = johnson_two_machine_order(first_machine, second_machine);
```

2機械flow shopの最適順序をJohnson法で返す。同値はjob番号順である。
時間$O(N\log N)$、追加領域$O(N)$。

## `neh_flow_shop_order`

```cpp
auto order = neh_flow_shop_order(processing_time);
```

総処理時間の降順にjobを取り出し、現在列の全位置へ試し挿入してmakespan最小の
位置を選ぶNEH heuristic。同値なら前の位置を選ぶ。
時間$O(N^3M)$、追加領域$O(N+M)$。

非長方形行列、負の処理時間、不正なjob列、2機械入力の長さ不一致では
`std::invalid_argument`を送出する。時刻加算は`Time`に収まる必要がある。
