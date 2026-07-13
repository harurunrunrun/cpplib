---
title: Parallel Machine List Scheduling (並列機械リストスケジューリング)
documentation_of: ../src/approximate/scheduling/parallel_machine.hpp
---

`approximate::scheduling`名前空間で、同一並列機械へjobを貪欲に割り当てる。

## `ParallelMachineSchedule`

```cpp
template<class Time>
struct ParallelMachineSchedule {
    vector<size_t> machine_of_job;
    vector<vector<size_t>> jobs_on_machine;
    vector<Time> machine_load;
    Time makespan;
};
```

各jobの割当機械、各機械での処理順、最終負荷、最大完了時刻を持つ。
member参照は$O(1)$、全割当列挙は$O(N+M)$である。

## `list_schedule`

```cpp
auto result = list_schedule(processing_time, machine_count, order);
```

`order`順に、現在負荷が最小の機械へjobを割り当てる。同負荷なら機械番号が
小さい方を選ぶ。`order`は全jobの置換でなければならない。

## `fifo_schedule` / `lifo_schedule`

job番号の昇順、降順をそれぞれlist schedulingへ渡す。

## `lpt_schedule` / `spt_schedule`

処理時間の降順（Longest Processing Time）、昇順（Shortest Processing Time）で
list schedulingを行う。同値はjob番号順である。LPTは同一並列機械のmakespanに対し
$4/3-1/(3M)$近似である。

## 計算量

$N$をjob数、$M$を機械数とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `list_schedule` | $O(N\log M+N+M)$ | $O(N+M)$ |
| `fifo_schedule`, `lifo_schedule` | $O(N\log M+N+M)$ | $O(N+M)$ |
| `lpt_schedule`, `spt_schedule` | $O(N\log N+N\log M+M)$ | $O(N+M)$ |

機械数0、負の処理時間、不正な置換では`std::invalid_argument`を送出する。
時刻の加算結果は`Time`に収まる必要がある。

## 注意点

処理時間行列と付随配列は各APIの形状を満たし、時間は有限かつ指定された符号条件を満たす必要がある。時刻の加算は `Time` の表現範囲内でなければならない。
