---
title: Unrelated-Machine Scheduling Heuristics
documentation_of: ../src/approximate/scheduling/unrelated_machine.hpp
---

`processing_time[job][machine]` で表す unrelated/heterogeneous machine へ job を割り当てる。

## 結果型

```cpp
template<class Time>
struct UnrelatedMachineSchedule {
    vector<size_t> machine_of_job;
    vector<vector<size_t>> jobs_on_machine;
    vector<Time> start_time, completion_time, machine_load;
    Time makespan;
};
```

各機械では `jobs_on_machine[m]` の順に idle なしで処理する。member 参照は $O(1)$、
全結果の列挙は $O(N+M)$。

## `unrelated_machine_schedule`

```cpp
enum class UnrelatedMachineRule { min_min, max_min, sufferage };
unrelated_machine_schedule(processing_time, rule);
```

未割当 job ごとに現在の機械負荷を含む最早完了時刻を計算し、`rule` に従って1 job を
割り当てる共通 API。同じ完了時刻では機械番号、同じ優先度では job 番号が小さいものを
選ぶ。時間 $O(N^2M)$、追加領域 $O(N+M)$。列挙子3値以外の `rule` では
`std::invalid_argument`。

## `min_min_schedule`

```cpp
min_min_schedule(processing_time);
```

各 job の最早完了時刻のうち最小のものを割り当てる Min-min。時間 $O(N^2M)$、
追加領域 $O(N+M)$。一般の unrelated-machine makespan に対する近似比は保証しない。

## `max_min_schedule`

```cpp
max_min_schedule(processing_time);
```

各 job の最早完了時刻のうち最大のものを割り当てる Max-min。時間 $O(N^2M)$、
追加領域 $O(N+M)$。一般の unrelated-machine makespan に対する近似比は保証しない。

## `sufferage_schedule`

```cpp
sufferage_schedule(processing_time);
```

2番目と1番目の完了時刻差が最大の job を割り当てる Sufferage。機械が1台なら差を
無限大として扱う。時間 $O(N^2M)$、追加領域 $O(N+M)$。一般の unrelated-machine
makespan に対する近似比は保証しない。

空の job 行列は機械数0の空 schedule を返す。job がある場合は1台以上必要である。
非長方形行列、負または非有限な処理時間では `std::invalid_argument`。ある job の全ての
候補完了時刻が `Time` から溢れる場合は `std::overflow_error`。
