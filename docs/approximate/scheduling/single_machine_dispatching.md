---
title: Dynamic Single-Machine Dispatching Rules (動的単一機械ディスパッチ規則)
documentation_of: ../../../src/approximate/scheduling/single_machine_dispatching.hpp
---

リリース時刻を持つ単一機械のジョブに対する動的ディスパッチ規則を `approximate::scheduling` 名前空間で提供する。同点はジョブ番号順である。

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

メンバの参照は $O(1)$、結果全体の列挙は $O(N+S)$。$S$ は割込み区間（segment）の数である。

## `shortest_remaining_processing_time_schedule`

```cpp
auto schedule = shortest_remaining_processing_time_schedule(
    processing_time, release_time = {}
);
```

利用可能なジョブの残処理時間が最小のものを処理し、新しいジョブのリリース時に再選択する SRPT。割込みを許す単一機械の完了時刻和を最小化する。連続する同一ジョブの区間は結合する。時間計算量は $O(N\log N)$、追加空間計算量は $O(N)$。

## `least_work_remaining_schedule`

```cpp
least_work_remaining_schedule(processing_time, release_time = {});
```

利用可能なジョブの処理時間が最小のものを選ぶ Least Work Remaining。時間計算量は $O(N^2)$、追加空間計算量は $O(N)$。リリース時刻付きの割込みなし問題に対するヒューリスティックである。

## `minimum_slack_time_schedule`

```cpp
minimum_slack_time_schedule(processing_time, due_date,
                            release_time = {});
```

利用可能なジョブの slack $d_i-t-p_i$ が最小のものを選ぶ Minimum Slack Time。時間計算量は $O(N^2)$、追加空間計算量は $O(N)$。一般の目的関数に対する近似比は保証しない。

## `critical_ratio_schedule`

```cpp
critical_ratio_schedule(processing_time, due_date,
                        release_time = {});
```

利用可能なジョブの critical ratio $(d_i-t)/p_i$ が最小のものを選ぶ。処理時間 0 のジョブは除算せず、時刻を進めずに正の処理時間のジョブより先に完了させる。時間計算量は $O(N^2)$、追加空間計算量は $O(N)$。一般の目的関数に対する近似比は保証しない。

## `highest_response_ratio_next_schedule`

```cpp
highest_response_ratio_next_schedule(processing_time,
                                     release_time = {});
```

利用可能なジョブの response ratio $1+(t-r_i)/p_i$ が最大のものを選ぶ HRRN。共通項 1 を省いて比較し、処理時間 0 のジョブは除算せずに優先する。時間計算量は $O(N^2)$、追加空間計算量は $O(N)$。一般の目的関数に対する近似比は保証しない。

## `apparent_tardiness_cost_schedule`

```cpp
apparent_tardiness_cost_schedule(
    processing_time, due_date, weight,
    look_ahead = 2, release_time = {}
);
```

利用可能なジョブの平均処理時間 $\bar{p}$ を用い、

$$
\frac{w_i}{p_i}\exp\left(-\frac{\max(d_i-t-p_i,0)}{k\bar{p}}\right)
$$

を最大化する ATC である。$k$ は `look_ahead` である。オーバーフローを避けるため対数の評価値を比較し、処理時間 0 のジョブは除算せずに優先する。時間計算量は $O(N^2)$、追加空間計算量は $O(N)$。重み付き納期遅れ用のヒューリスティックであり、近似比は保証しない。

## `apparent_tardiness_cost_with_setups_schedule`

```cpp
apparent_tardiness_cost_with_setups_schedule(
    processing_time, due_date, weight,
    setup_time, initial_setup_time,
    look_ahead = 2, setup_look_ahead = 1,
    release_time = {}
);
```

ATC の評価値に、直前のジョブ $j$ からの段取り時間 $s_{ji}$ による $\exp(-s_{ji}/(k_s \bar{s}))$ も掛ける ATCS である。$k_s$ は `setup_look_ahead`、$\bar{s}$ は利用可能なジョブへの正の段取り時間の平均である（0 なら段取りの因子は 1）。先頭のジョブには `initial_setup_time[i]` を使い、`start_time[i]` は段取り完了後の処理開始時刻である。処理時間 0 のジョブは除算せずに優先し、同点なら段取り時間が短いジョブを選ぶ。入力検査を含め時間計算量は $O(N^2)$、追加空間計算量は $O(N)$（段取り時間の入力自体は $O(N^2)$）。重み付き納期遅れ用のヒューリスティックであり、近似比は保証しない。

## 注意点

- 省略した `release_time` はすべて 0 として扱う。
- 配列の長さの不一致、負または有限でない処理時間・リリース時刻・段取り時間、有限でない納期、正でない重み・パラメータ、正方でない段取り時間行列に対しては `std::invalid_argument` を送出する。
- 時刻の加算が `Time` に収まらない場合は `std::overflow_error` を送出する。
