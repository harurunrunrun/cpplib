---
title: Round-Robin Completion Times (ラウンドロビン完了時刻) [RRSCHED]
documentation_of: ../src/algorithm/other/scheduling/round_robin_completion_times.hpp
---

各タスクを添字順に1単位ずつ処理するラウンドロビン方式について、各タスクが完了する時刻を返す。

## round_robin_completion_times

```cpp
template<int MAX_TASKS = 50000>
vector<long long> round_robin_completion_times(
    const vector<long long>& processing_times
);
```

`processing_times[i]` をタスク `i` に必要な正の処理単位数とする。時刻0から、未完了タスクを添字順に1単位ずつ処理したときの各完了時刻を返す。

## 時間計算量

タスク数を $N$ として $O(N\log N)$。

## 空間計算量

$O(N)$。

## 注意点

- 全要素は正でなければならない。
- タスク数は `MAX_TASKS` 以下でなければならない。
- 前提違反時は `runtime_error` を送出する。
- 完了時刻は `long long` で表現できる必要がある。
