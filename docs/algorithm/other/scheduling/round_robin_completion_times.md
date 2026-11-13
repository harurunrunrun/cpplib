---
title: Round-Robin Completion Times (ラウンドロビン完了時刻) [RRSCHED]
documentation_of: ../../../../src/algorithm/other/scheduling/round_robin_completion_times.hpp
---

各タスクを添字順に 1 単位ずつ処理するラウンドロビン方式について、各タスクが完了する時刻を返す。

## round_robin_completion_times

```cpp
template<int MAX_TASKS = -1>
vector<long long> round_robin_completion_times(
    const vector<long long>& processing_times
);
```

`processing_times[i]` をタスク `i` に必要な正の処理単位数とする。時刻 $0$ から、未完了のタスクを添字順に 1 単位ずつ処理したときの各完了時刻を返す。

`MAX_TASKS` を省略すると、内部の Fenwick 木に実行時サイズの `std::vector` を使う可変長版になる。指定すると固定長版になる。

```cpp
auto result = round_robin_completion_times(processing_times);
auto fixed = round_robin_completion_times<100000>(processing_times);
```

## 時間計算量

タスク数を $N$ として $O(N\log N)$。

## 空間計算量

可変長版は $O(N)$、固定長版は $O(\texttt{MAX_TASKS})$。

## 注意点

- 全要素は正でなければならない。違反時は `runtime_error` を送出する。
- 固定長版ではタスク数が `MAX_TASKS` 以下でなければならない。
- 完了時刻は `long long` で表現できなければならない。
