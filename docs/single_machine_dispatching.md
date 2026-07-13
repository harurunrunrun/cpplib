---
title: Dynamic Single-Machine Dispatching Rules (動的単一機械ディスパッチ規則)
documentation_of: ../src/approximate/scheduling/single_machine_dispatching.hpp
---

release time を持つ単一機械 job の動的 dispatching rule を
`approximate::scheduling` 名前空間で提供する。同点は job 番号順である。

## 結果型

```cpp
template<class Time>
struct SingleMachineSchedule {
    vector<size_t> order;
    vector<Time> start_time, completion_time;
    Time makespan;
};

template<class Time>
struct PreemptiveSegment {
    size_t job;
    Time start, end;
};

template<class Time>
struct PreemptiveSingleMachineSchedule {
    vector<PreemptiveSegment<Time>> segments;
    vector<Time> completion_time;
    Time makespan;
};
```

member 参照は $O(1)$、結果全体の列挙は $O(N+S)$。$S$ は preemptive segment 数である。

## `shortest_remaining_processing_time_schedule`

```cpp
auto schedule = shortest_remaining_processing_time_schedule(
    processing_time, release_time = {}
);
```

利用可能 job の残処理時間が最小のものを処理し、新しい job の release 時に再選択する
SRPT。preemption を許す単一機械の完了時刻和を最小化する。連続する同一 job の segment は
結合する。時間計算量は $O(N\log N)$、追加空間計算量は $O(N)$。

## `least_work_remaining_schedule`

```cpp
least_work_remaining_schedule(processing_time, release_time = {});
```

利用可能 job の処理時間が最小のものを選ぶ Least Work Remaining。時間計算量は $O(N^2)$、
追加空間計算量は $O(N)$。release time 付き非 preemptive 問題に対する heuristic である。

## `minimum_slack_time_schedule`

```cpp
minimum_slack_time_schedule(processing_time, due_date,
                            release_time = {});
```

利用可能 job の slack $d_i-t-p_i$ が最小のものを選ぶ Minimum Slack Time。
時間計算量は $O(N^2)$、追加空間計算量は $O(N)$。一般の目的関数に対する近似比は保証しない。

## `critical_ratio_schedule`

```cpp
critical_ratio_schedule(processing_time, due_date,
                        release_time = {});
```

利用可能 job の critical ratio $(d_i-t)/p_i$ が最小のものを選ぶ。処理時間0は除算せず、
有限時間 job より先に時刻を進めず完了させる。時間計算量は $O(N^2)$、追加空間計算量は $O(N)$。
一般の目的関数に対する近似比は保証しない。

## `highest_response_ratio_next_schedule`

```cpp
highest_response_ratio_next_schedule(processing_time,
                                     release_time = {});
```

利用可能 job の response ratio $1+(t-r_i)/p_i$ が最大のものを選ぶ HRRN。共通項1を
省いて比較し、処理時間0は除算せず優先する。時間計算量は $O(N^2)$、追加空間計算量は $O(N)$。
一般の目的関数に対する近似比は保証しない。

## `apparent_tardiness_cost_schedule`

```cpp
apparent_tardiness_cost_schedule(
    processing_time, due_date, weight,
    look_ahead = 2, release_time = {}
);
```

利用可能 job の平均処理時間 $\bar p$ を用い、

$$
\frac{w_i}{p_i}\exp\left(-\frac{\max(d_i-t-p_i,0)}{k\bar p}\right)
$$

を最大化する ATC。overflow を避けるため対数 score を比較し、処理時間0は除算せず
優先する。時間計算量は $O(N^2)$、追加空間計算量は $O(N)$。重み付き tardiness 用 heuristic であり、
近似比は保証しない。

## `apparent_tardiness_cost_with_setups_schedule`

```cpp
apparent_tardiness_cost_with_setups_schedule(
    processing_time, due_date, weight,
    setup_time, initial_setup_time,
    look_ahead = 2, setup_look_ahead = 1,
    release_time = {}
);
```

ATC score に、直前 job $j$ からの setup $s_{ji}$ による
$\exp(-s_{ji}/(k_s\bar s))$ も掛ける ATCS。先頭 job には
`initial_setup_time[i]` を使い、`start_time[i]` は setup 完了後の処理開始時刻である。
処理時間0は除算せず優先し、同点は setup が短い job を選ぶ。入力検査を含め時間計算量は $O(N^2)$、追加空間計算量は $O(N)$（setup 入力自体は $O(N^2)$）。重み付き tardiness 用
heuristic であり、近似比は保証しない。

省略した `release_time` は全て0として扱う。配列長不一致、負または非有限な処理・release・
setup 時間、非有限な納期、非正の重み・parameter、非正方 setup 行列では
`std::invalid_argument`。時刻加算が `Time` に収まらない場合は `std::overflow_error`。

## 注意点

処理時間行列と付随配列は各APIの形状を満たし、時間は有限かつ指定された符号条件を満たす必要がある。時刻の加算は `Time` の表現範囲内でなければならない。
