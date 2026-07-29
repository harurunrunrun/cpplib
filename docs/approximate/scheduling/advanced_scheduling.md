---
title: Advanced Scheduling Heuristics (高度なスケジューリングヒューリスティック)
documentation_of: ../../../src/approximate/scheduling/advanced_scheduling.hpp
---

## `shifting_bottleneck_schedule`

```cpp
template<class Time>
struct JobShopOperation {
    size_t machine;
    Time processing_time;
};

shifting_bottleneck_schedule(jobs, machine_count);
```

各 job の operation 順を precedence とし、未固定 machine ごとに現在 DAG の
最早開始時刻 $r$ と後続 tail $q$ を計算する。$1|r_i|L_{\max}$ の Schrage
順序を作り、最大 bound の machine を bottleneck として固定する。候補順序が
既存 precedence と cycle を作る場合は現在の topological 順へ戻して合法性を
保つ。

`ShiftingBottleneckSchedule` は operation ごとの `start_time` /
`completion_time`、各 `machine_order`、machine の `bottleneck_order`、
`makespan` を返す。operation 数を $V$、precedence 辺数を $E$、machine 数を
$M$ とすると時間計算量は $O(M^2(V\log V+E))$、空間計算量は $O(V+E+M)$。

## `randomized_list_schedule`

```cpp
randomized_list_schedule(processing_time, machine_count, random);
```

job 順を一様 shuffle してから最小負荷 machine へ list scheduling する。
時間計算量は $O(N+N\log M)$、空間計算量は $O(N+M)$。

## `multifit_schedule`

```cpp
multifit_schedule(processing_time, machine_count, iterations = 10);
```

LPT makespan を上界、平均負荷・最大 job・MultiFit 係数による値を下界とし、
capacity を二分探索する。各 capacity で First-Fit Decreasing が $M$ bin
以内か判定する。`MultiFitScheduleResult` は最良 `schedule`、最後の feasible
`fitted_capacity`、反復数を返す。

時間計算量は $O(I(N\log N+NM))$、空間計算量は $O(N+M)$。

## 注意点

machine 数は正、処理時間は非負有限値でなければならない。job-shop の machine
添字範囲外では `std::out_of_range`。整数時刻の加算 overflow、浮動小数点の
非有限結果では `std::overflow_error`。Shifting Bottleneck と MultiFit は
heuristic であり、一般 job-shop / multiprocessor instance の最適性を保証しない。
