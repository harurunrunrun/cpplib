---
title: Unrelated-Machine Scheduling Heuristics (無関連機械スケジューリングヒューリスティック)
documentation_of: ../../../src/approximate/scheduling/unrelated_machine.hpp
---

`processing_time[job][machine]` で表す無関連（unrelated / heterogeneous）機械へジョブを割り当てる。ジョブ数を $N$、機械数を $M$ とする。

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

各機械は `jobs_on_machine[m]` の順に、空き時間なしで処理する。メンバの参照は $O(1)$、全結果の列挙は $O(N + M)$。

## `unrelated_machine_schedule`

```cpp
enum class UnrelatedMachineRule { min_min, max_min, sufferage };
unrelated_machine_schedule(processing_time, rule);
```

未割当ての各ジョブについて現在の機械負荷を含めた最早完了時刻を計算し、`rule` に従って 1 ジョブを割り当てることを繰り返す共通 API である。完了時刻が同じなら機械番号が、優先度が同じならジョブ番号が小さいものを選ぶ。
時間計算量は $O(N^2 M)$、追加空間計算量は $O(N + M)$。3 つの列挙子以外の値の `rule` には `std::invalid_argument` を送出する。

## `min_min_schedule`

```cpp
min_min_schedule(processing_time);
```

各ジョブの最早完了時刻のうち、最小のものを持つジョブを割り当てる Min-min 法。時間計算量は $O(N^2 M)$、追加空間計算量は $O(N + M)$。

## `max_min_schedule`

```cpp
max_min_schedule(processing_time);
```

各ジョブの最早完了時刻のうち、最大のものを持つジョブを割り当てる Max-min 法。時間計算量は $O(N^2 M)$、追加空間計算量は $O(N + M)$。

## `sufferage_schedule`

```cpp
sufferage_schedule(processing_time);
```

2 番目に早い完了時刻と最早完了時刻の差が最大のジョブを割り当てる Sufferage 法。機械が 1 台なら差を無限大として扱う。時間計算量は $O(N^2 M)$、追加空間計算量は $O(N + M)$。

## 注意点

- いずれも、一般の無関連機械の makespan に対する近似比は保証しない。
- 空のジョブ行列には、機械数 0 の空のスケジュールを返す。ジョブがある場合は機械が 1 台以上必要である。
- 長方形でない行列、負または有限でない処理時間に対しては `std::invalid_argument` を送出する。あるジョブの候補完了時刻がすべて `Time` からあふれる場合は `std::overflow_error` を送出する。
