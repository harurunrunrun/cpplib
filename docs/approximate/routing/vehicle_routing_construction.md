---
title: Vehicle Routing Construction Heuristics (配送経路構築ヒューリスティック)
documentation_of: ../../../src/approximate/routing/vehicle_routing_construction.hpp
---

容量制約付き配送経路と時間枠付き配送経路を構築する。各routeはdepotを含まず、depotから出発して列の顧客を訪問しdepotへ戻るものとして扱う。

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

互いに離れたseedを選び、各顧客の第1・第2候補cluster間の割当regretが大きい順に容量を満たすvehicleへ割り当てる。各cluster内では巡回距離の増分が最小の位置へ顧客を挿入する。空vehicleも返り値に残る。

- 時間計算量: $O(N^2K+N^3)$（`K = vehicle_count`）
- 追加空間計算量: $O(N+K)$

## `RoutingTimeWindow`

~~~cpp
struct RoutingTimeWindow {
    long double earliest;
    long double latest;
    long double service_time;
};
~~~

到着可能時刻区間とサービス時間を表す。早着時は `earliest` まで待機する。depotの時間枠は各routeの出発可能時刻と帰着期限に使う。

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

期限が早い実行可能顧客からrouteを開始し、距離増分・帰着時刻の遅延・待機時間の増分を合わせたMole–Jameson基準が最小の実行可能挿入を反復する。

- 時間計算量: $O(N^4)$
- 追加空間計算量: $O(N^2)$（返すrouteを含む）

## `solomon_i1_routes`, `solomon_i2_routes`, `solomon_i3_routes`

~~~cpp
vector<vector<int>> solomon_i1_routes(/* 同じ引数 */);
vector<vector<int>> solomon_i2_routes(/* 同じ引数 */);
vector<vector<int>> solomon_i3_routes(/* 同じ引数 */);
~~~

実行可能な全挿入について距離増分と時刻増分から `c1` を作り、depotから遠い顧客を優先する `c2 = lambda * distance(depot,u) - c1` が最大のものを選ぶ。
I1は距離増分、I2は時刻増分、I3は両者の平均を主に評価し、I3では遠方顧客の係数を2にする。

- 各APIの時間計算量: $O(N^4)$
- 各APIの追加空間計算量: $O(N^2)$（返すrouteを含む）

## 注意点

距離行列は非空の正方行列、`demand` と `time_window` は頂点数と同じ長さでなければならない。容量・需要・サービス時間は有限かつ非負、時間枠は有限の `earliest` と `latest >= earliest` を要求する。違反は `std::invalid_argument`、範囲外depotは `std::out_of_range`。
`N > INT_MAX`: `std::length_error`.
Nonzero `demand[depot]`: `std::invalid_argument`.
単独往復不能な顧客、容量超過顧客、fleet容量不足、またはgreedy割当が容量を満たせない場合は `std::domain_error`。距離値を移動時間としても使う。
