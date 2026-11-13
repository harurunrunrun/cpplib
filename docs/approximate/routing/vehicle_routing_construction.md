---
title: Vehicle Routing Construction Heuristics (配送経路構築ヒューリスティック)
documentation_of: ../../../src/approximate/routing/vehicle_routing_construction.hpp
---

容量制約付きの配送経路と、時間枠付きの配送経路を構築する。各経路はデポ（`depot`）を含まず、デポを出発して列の顧客を順に訪問し、デポへ戻るものとして扱う。頂点数を $N$ とする。

## `fisher_jaikumar_routes`

~~~cpp
vector<vector<int>> fisher_jaikumar_routes(
    const DistanceMatrix& distance,
    const vector<long double>& demand,
    long double vehicle_capacity,
    size_t vehicle_count,
    size_t depot = 0
)
~~~

互いに離れた種顧客（seed）を選び、各顧客について第 1 候補と第 2 候補のクラスタの間の割当コストの差（regret）が大きい順に、容量を満たす車両へ割り当てる。各クラスタ内では、巡回距離の増分が最小の位置へ顧客を挿入する。空の車両の経路も返値に残る。

- 時間計算量: $K$ を `vehicle_count` として $O(N^2 K + N^3)$
- 追加空間計算量: $O(N+K)$

## `RoutingTimeWindow`

~~~cpp
struct RoutingTimeWindow {
    long double earliest;
    long double latest;
    long double service_time;
};
~~~

到着可能な時刻の区間とサービス時間を表す。早く着いた場合は `earliest` まで待機する。デポの時間枠は、各経路の出発可能時刻と帰着期限に使う。

## `mole_jameson_routes`

~~~cpp
vector<vector<int>> mole_jameson_routes(
    const DistanceMatrix& distance,
    const vector<long double>& demand,
    long double vehicle_capacity,
    const vector<RoutingTimeWindow>& time_window,
    size_t depot = 0
)
~~~

期限が早い実行可能な顧客から経路を始め、距離の増分・帰着時刻の遅れ・待機時間の増分を合わせた Mole–Jameson 基準が最小となる実行可能な挿入を繰り返す。

- 時間計算量: $O(N^4)$
- 追加空間計算量: 返す経路を含めて $O(N^2)$

## `solomon_i1_routes`, `solomon_i2_routes`, `solomon_i3_routes`

~~~cpp
vector<vector<int>> solomon_i1_routes(/* 同じ引数 */);
vector<vector<int>> solomon_i2_routes(/* 同じ引数 */);
vector<vector<int>> solomon_i3_routes(/* 同じ引数 */);
~~~

実行可能なすべての挿入について、距離の増分と時刻の増分から `c1` を作り、デポから遠い顧客を優先する `c2 = lambda * distance(depot, u) - c1` が最大のものを選ぶ。I1 は距離の増分、I2 は時刻の増分、I3 は両者の平均で `c1` を評価し、`lambda` は I1・I2 で 1、I3 で 2 である。

- 各 API の時間計算量: $O(N^4)$
- 各 API の追加空間計算量: 返す経路を含めて $O(N^2)$

## 注意点

- 距離行列は空でない正方行列で、`demand` と `time_window` は頂点数と同じ長さでなければならない。容量・需要・サービス時間は有限かつ非負で、時間枠は有限の `earliest` と `latest >= earliest` を要求する。`demand[depot]` は 0 でなければならず、`fisher_jaikumar_routes` では `vehicle_count` が正でなければならない。これらの違反には `std::invalid_argument` を送出する。
- 範囲外の `depot` には `std::out_of_range` を、$N$ が `INT_MAX` を超える場合は `std::length_error` を送出する。
- 単独で往復できない顧客や容量を超える顧客がある場合、車両全体の容量が不足する場合、または貪欲な割当てで容量を満たせない場合は `std::domain_error` を送出する。
- 距離の値は移動時間としても使う。
