---
title: Parallel Machine List Scheduling (並列機械リストスケジューリング)
documentation_of: ../../../src/approximate/scheduling/parallel_machine.hpp
---

`approximate::scheduling` 名前空間で、同一の並列機械へジョブを貪欲に割り当てる。ジョブ数を $N$、機械数を $M$ とする。

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

各ジョブの割当先の機械、各機械での処理順、各機械の最終負荷、最大完了時刻を持つ。
メンバの参照は $O(1)$、全割当ての列挙は $O(N + M)$ である。

## `list_schedule`

```cpp
auto result = list_schedule(processing_time, machine_count, order);
```

`order` の順に、現在の負荷が最小の機械へジョブを割り当てる。負荷が同じなら機械番号が小さい方を選ぶ。`order` は全ジョブの順列でなければならない。

## `fifo_schedule` / `lifo_schedule`

ジョブ番号の昇順・降順を、それぞれリストスケジューリングに渡す。

## `lpt_schedule` / `spt_schedule`

処理時間の降順（Longest Processing Time）・昇順（Shortest Processing Time）でリストスケジューリングを行う。同値ならジョブ番号順である。LPT は同一並列機械の makespan に対して $4/3 - 1/(3M)$ 近似である。

## 時間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `list_schedule` | $O(N \log M + N + M)$ | $O(N + M)$ |
| `fifo_schedule`, `lifo_schedule` | $O(N \log M + N + M)$ | $O(N + M)$ |
| `lpt_schedule`, `spt_schedule` | $O(N \log N + N \log M + M)$ | $O(N + M)$ |

## 注意点

- 機械数 0、負の処理時間、不正な順列に対しては `std::invalid_argument` を送出する。
- 時刻の加算結果は `Time` に収まらなければならない。
