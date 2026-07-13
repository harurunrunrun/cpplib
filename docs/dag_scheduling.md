---
title: DAG・異種プロセッサスケジューリング
documentation_of: ../src/approximate/scheduling/dag_scheduling.hpp
---

タスクごと・プロセッサごとに処理時間が異なるDAGを、挿入型リストスケジューリングで
割り当てるヒューリスティック。辺の通信時間は、両端を異なるプロセッサへ割り当てたとき
だけ加算し、同じプロセッサなら0とする。

以下でタスク数を $V$、プロセッサ数を $P$、辺数を $E$ とする。

## `DagCommunicationEdge`

```cpp
template<class Time>
struct DagCommunicationEdge {
    size_t from, to;
    Time communication_time;
};
```

`from -> to` の先行制約と、異なるプロセッサ間で必要な通信時間を表す。構築と各member
参照は $O(1)$。

## `DagTaskSlot`

```cpp
template<class Time>
struct DagTaskSlot {
    size_t task;
    Time start, finish;
};
```

1タスクの予約区間 `[start, finish)` を表す。構築と各member参照は $O(1)$。

## `DagSchedule`

```cpp
template<class Time>
struct DagSchedule {
    vector<size_t> processor_of_task;
    vector<Time> start_time, finish_time;
    vector<size_t> scheduling_order;
    vector<vector<DagTaskSlot<Time>>> tasks_on_processor;
    vector<long double> priority;
    vector<size_t> critical_path;
    size_t critical_processor;
    Time makespan;
};
```

`processor_of_task[v]`、`start_time[v]`、`finish_time[v]` はタスク `v` の割当結果。
`scheduling_order` は優先度付きready queueから取り出した順であり、実行開始時刻順とは限らない。
`tasks_on_processor[p]` は開始時刻順の予約で、既存予約間の最初の十分な隙間も利用する。
`priority` は処理時間と通信時間全体の最大値（全て0なら1）を共通scaleとした、各手法の
正規化済み優先度である。このscaleは優先順位を変えずrankのoverflowを避ける。CPOPだけ
`critical_path` と
`critical_processor` を設定し、他手法では空列と `numeric_limits<size_t>::max()` になる。
各member参照は $O(1)$、全割当の列挙は $O(V+P)$。

## `critical_path_priority_schedule`

```cpp
critical_path_priority_schedule(processing_time, edges);
```

各タスクの最小処理時間をノード重みとした上向きrank（そのタスクから出口までの最長長）を
優先度に使う Critical-Path priority scheduling。readyなタスクを優先度降順で選び、全
プロセッサのうち挿入後の完了時刻が最小となるものへ割り当てる。

時間 $O(V^2+P(V+E)+V\log V)$、入力を除く追加領域 $O(V+E+P)$。
一般のDAGスケジューリングに対する近似比は保証しない。

## `heft_schedule`

```cpp
heft_schedule(processing_time, edges);
```

各タスクの全プロセッサ上の算術平均処理時間と通信時間から HEFT の上向きrankを求め、
その降順で挿入型の最早完了プロセッサへ割り当てる。通信時間は入力値をrankにも用いる。

時間 $O(V^2+P(V+E)+V\log V)$、入力を除く追加領域 $O(V+E+P)$。
一般のDAGスケジューリングに対する近似比は保証しない。

## `cpop_schedule`

```cpp
cpop_schedule(processing_time, edges);
```

平均処理時間から上向きrankと下向きrankを求め、その和を優先度に使う CPOP。最大上向き
rankの入口から `communication_time + successor_upward_rank` が最大の辺をたどって
critical pathを定め、そのpathの処理時間合計が最小の1プロセッサへpath上の全タスクを
固定する。それ以外は挿入型の最早完了プロセッサへ割り当てる。

時間 $O(V^2+P(V+E)+V\log V)$、入力を除く追加領域 $O(V+E+P)$。
一般のDAGスケジューリングに対する近似比は保証しない。

3手法とも通信路自体の資源競合はモデル化しない。異なる複数の辺の通信は互いに並行できる
ものとし、各後続タスクのready時刻だけを制約する。

## 入力検査・決定性

`processing_time[task][processor]` と `communication_time` は非負かつ有限でなければ
ならない。タスクがあるときは $P>0$ が必要である。非長方形行列、範囲外の辺端点、
自己loopを含むcycle、負値、NaN、無限大では `std::invalid_argument` を投げる。
空のタスク集合と空の辺集合には空scheduleを返す。

同優先度ならタスク番号、同じ完了時刻なら開始時刻、さらに同じならプロセッサ番号が
小さい候補を選ぶ。CPOPのpath・固定プロセッサも同値なら番号の小さいものを選ぶため、
同じ入力には決定的な結果を返す。予約時刻や通信到着時刻が `Time` に収まらない候補は
除外し、あるタスクに表現可能な候補が1つもなければ `std::overflow_error` を投げる。
