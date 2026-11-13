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

各ジョブの工程（operation）の順序を先行制約とし、順序が未固定の機械ごとに、現在の DAG での最早開始時刻 $r$ と後続の tail $q$ を計算する。$1 \mid r_i \mid L_{\max}$ に対する Schrage の順序を作り、評価値が最大の機械をボトルネックとして順序を固定する。候補の順序が既存の先行制約と閉路を作る場合は、現在のトポロジカル順に戻して実行可能性を保つ。

`ShiftingBottleneckSchedule` は、工程ごとの `start_time` / `completion_time`、各機械の `machine_order`、機械を固定した順 `bottleneck_order`、`makespan` を返す。
工程数を $V$、先行制約の辺数を $E$、機械数を $M$ とすると、時間計算量は $O(M^2 (V \log V + E))$、空間計算量は $O(V + E + M)$。

## `randomized_list_schedule`

```cpp
randomized_list_schedule(processing_time, machine_count, random);
```

ジョブの順序を一様にシャッフルしてから、負荷が最小の機械へ割り当てるリストスケジューリングを行う。
ジョブ数を $N$ として、時間計算量は $O(N + N \log M)$、空間計算量は $O(N + M)$。

## `multifit_schedule`

```cpp
multifit_schedule(processing_time, machine_count, iterations = 10);
```

LPT の makespan を上界、平均負荷・最大ジョブ・MultiFit 係数による値を下界として、容量を二分探索する。各容量で First-Fit Decreasing が $M$ 個以内のビンに収まるかを判定する。
`MultiFitScheduleResult` は、最良のスケジュール `schedule`、最後に実行可能だった容量 `fitted_capacity`、反復回数 `iterations` を返す。

反復回数を $I$ として、時間計算量は $O(I(N \log N + NM))$、空間計算量は $O(N + M)$。

## 注意点

- 機械数は正、処理時間は非負の有限値でなければならず、違反時は `std::invalid_argument` を送出する。
- ジョブショップの機械番号が範囲外なら `std::out_of_range` を送出する。
- 整数時刻の加算のオーバーフローや、浮動小数点の結果が有限でない場合は `std::overflow_error` を送出する。
- Shifting Bottleneck と MultiFit はヒューリスティックであり、一般のジョブショップ・マルチプロセッサの問題に対する最適性は保証しない。
