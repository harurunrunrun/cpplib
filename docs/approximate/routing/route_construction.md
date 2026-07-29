---
title: Constructive Routing Heuristics (経路構築ヒューリスティック)
documentation_of: ../../../src/approximate/routing/route_construction.hpp
---

完全距離行列から TSP の巡回順、容量制約付き配送経路、または顧客の並びを決定的に構築する。
容量制約付き API が返す各経路は depot を含まない。実際の巡回は
`depot -> route[0] -> ... -> route.back() -> depot` である。

## `RoutingPoint`

~~~cpp
struct RoutingPoint {
    long double x;
    long double y;
};
~~~

角度に基づく構築法で用いる二次元座標である。

## Sequential Insertion

~~~cpp
template<class DistanceMatrix, class DemandContainer>
vector<vector<int>> sequential_insertion_routes(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    demand_value_t<DemandContainer> capacity,
    size_t depot = 0
)
~~~

1 本の経路を完成させてから次の経路を作る逐次挿入法である。
現在の経路に積載可能な全顧客・全位置から距離増分が最小の挿入を選ぶ。
積載可能な未処理顧客がなくなるとその経路を確定する。
同値なら顧客番号、挿入位置が小さい候補を優先する。

- 時間計算量: 顧客数を \(N\) として \(O(N^3)\)
- 追加空間計算量: \(O(N)\)

## Parallel Insertion

~~~cpp
template<class DistanceMatrix, class DemandContainer>
vector<vector<int>> parallel_insertion_routes(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    demand_value_t<DemandContainer> capacity,
    size_t vehicle_count,
    size_t depot = 0
)
~~~

depot との往復距離が大きい顧客から最大 `vehicle_count` 本の seed 経路を作り、
全経路を並行して拡張する。各反復では積載可能な顧客・経路・位置のうち
距離増分が最小のものを選ぶ。返す経路は空経路を含まない。

- 時間計算量: \(O(N^3)\)
- 追加空間計算量: \(O(N)\)

greedy な割当てで残りの顧客を収容できない場合は、総需要だけからは実行可能でも
`std::invalid_argument` を送出する。

## Multi-Fragment

~~~cpp
template<class DistanceMatrix>
vector<int> multi_fragment_tour(const DistanceMatrix& distance)
~~~

全無向辺を軽い順に調べ、頂点次数を 2 以下に保ち、途中で閉路を作らない辺を
\(N-1\) 本選ぶ。得られた Hamilton path の両端を暗黙に結んだ TSP 巡回順を返す。
同じ重みでは端点番号の辞書順を使う。辺重みには `distance[i][j]` (`i < j`) を使う。

- 時間計算量: \(O(N^2\log N)\)
- 追加空間計算量: \(O(N^2)\)

## Clarke–Wright / Parallel Savings

~~~cpp
template<class DistanceMatrix, class DemandContainer>
vector<vector<int>> clarke_wright_routes(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    demand_value_t<DemandContainer> capacity,
    size_t depot = 0
)

template<class DistanceMatrix, class DemandContainer>
vector<vector<int>> parallel_savings_routes(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    demand_value_t<DemandContainer> capacity,
    size_t depot = 0
)
~~~

各顧客を単独経路として開始し、
\[
s(i,j)=d(i,depot)+d(depot,j)-d(i,j)
\]
を大きい順に処理する Parallel Savings 法である。
異なる経路の端点同士で、併合後も容量以内となる非負 saving を採用する。
必要なら経路を反転して `i` の直後に `j` を接続する。
`clarke_wright_routes` は `parallel_savings_routes` と同じ実装を呼び出す。

- 時間計算量: \(O(N^2\log N)\)
- 追加空間計算量: \(O(N^2)\)

## Sequential Savings

~~~cpp
template<class DistanceMatrix, class DemandContainer>
vector<vector<int>> sequential_savings_routes(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    demand_value_t<DemandContainer> capacity,
    size_t depot = 0
)
~~~

未処理の最小番号の顧客から 1 本の経路を開始し、その両端のうち saving が最大となる
顧客を順に接続する。非負 saving の接続がなくなるか容量に達した後、次の経路を作る。

- 時間計算量: \(O(N^2)\)
- 追加空間計算量: \(O(N)\)

## Angle-First

~~~cpp
vector<int> angle_first_order(
    const vector<RoutingPoint>& points,
    size_t depot = 0
)
~~~

depot から見た偏角の昇順に顧客を並べる。偏角が同じ場合は depot からの
二乗距離、さらに頂点番号で tie-break する。返り値は depot を含まない。

- 時間計算量: \(O(N\log N)\)
- 追加空間計算量: \(O(N)\)

## Sweep

~~~cpp
template<class DistanceMatrix, class DemandContainer>
vector<vector<int>> sweep_routes(
    const DistanceMatrix& distance,
    const vector<RoutingPoint>& points,
    const DemandContainer& demand,
    demand_value_t<DemandContainer> capacity,
    size_t depot = 0
)
~~~

`angle_first_order` の順に顧客を走査し、次の顧客を積むと容量を超える位置で
経路を区切る Sweep 法である。経路内の順番は偏角順を保つ。

- 時間計算量: \(O(N\log N)\)
- 追加空間計算量: \(O(N)\)

## Petal

~~~cpp
template<class DistanceMatrix, class DemandContainer>
vector<vector<int>> petal_routes(
    const DistanceMatrix& distance,
    const vector<RoutingPoint>& points,
    const DemandContainer& demand,
    demand_value_t<DemandContainer> capacity,
    size_t depot = 0
)
~~~

偏角順の全循環 shift を花弁候補とし、それぞれを容量で区切る。
全閉路距離の合計が最小の候補を返す。同値では shift が小さい候補を残す。

- 時間計算量: \(O(N^2)\)
- 追加空間計算量: \(O(N)\)

## Cluster-First Route-Second

~~~cpp
template<class DistanceMatrix, class DemandContainer>
vector<vector<int>> cluster_first_route_second(
    const DistanceMatrix& distance,
    const vector<RoutingPoint>& points,
    const DemandContainer& demand,
    demand_value_t<DemandContainer> capacity,
    size_t depot = 0
)
~~~

Sweep 法で容量を満たす顧客 cluster を作り、各 cluster 内を cheapest insertion で
並べ直す。cluster 間で顧客を移動しない。

- 時間計算量: \(O(N^2)\)
- 追加空間計算量: \(O(N)\)

## Giant Tour

~~~cpp
template<class DistanceMatrix>
vector<int> giant_tour(
    const DistanceMatrix& distance,
    size_t depot = 0
)
~~~

depot から最近傍法を開始して全顧客を 1 列に並べる。返り値に depot は含まない。
距離が同じ場合は頂点番号が小さい顧客を選ぶ。

- 時間計算量: \(O(N^2)\)
- 追加空間計算量: \(O(N)\)

## Split

~~~cpp
template<class DistanceMatrix, class DemandContainer>
vector<vector<int>> split_tour(
    const DistanceMatrix& distance,
    const vector<int>& giant_order,
    const DemandContainer& demand,
    demand_value_t<DemandContainer> capacity,
    size_t depot = 0
)
~~~

全顧客を一度ずつ含む `giant_order` を、順序を保った容量内の連続区間へ分割する。
各区間を depot から出発して depot に戻る経路としたときの総距離を
動的計画法で最小化する。

- 時間計算量: \(O(N^2)\)
- 追加空間計算量: 返り値を除いて \(O(N)\)

## Route-First Cluster-Second

~~~cpp
template<class DistanceMatrix, class DemandContainer>
vector<vector<int>> route_first_cluster_second(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    demand_value_t<DemandContainer> capacity,
    size_t depot = 0
)
~~~

`giant_tour` で顧客順を作ってから `split_tour` で容量内の経路へ分割する。

- 時間計算量: \(O(N^2)\)
- 追加空間計算量: \(O(N)\)

## Nearest Depot

~~~cpp
template<class DistanceMatrix>
vector<int> nearest_depot_assignment(
    const DistanceMatrix& distance,
    const vector<int>& depots
)
~~~

各非 depot 頂点を `distance[vertex][depot]` が最小の depot 頂点へ割り当てる。
返り値の `assignment[vertex]` は depot の頂点番号である。
depot 自身は自分自身へ割り当てる。同距離では `depots` 内で先に現れる depot を選ぶ。
空行列に空の `depots` を渡した場合は空列を返す。

- 時間計算量: 頂点数を \(N\)、depot 数を \(D\) として \(O(ND)\)
- 追加空間計算量: \(O(N)\)

## Path Cheapest Arc

~~~cpp
template<class DistanceMatrix>
vector<int> path_cheapest_arc_tour(const DistanceMatrix& distance)
~~~

全有向 arc を軽い順に調べ、各頂点の入次数・出次数を 1 以下に保ち、
途中で有向閉路を作らない \(N-1\) 本を選ぶ。得られた Hamilton path の順を返す。
巡回路として使う場合は末尾から先頭への辺を暗黙に追加する。

- 時間計算量: \(O(N^2\log N)\)
- 追加空間計算量: \(O(N^2)\)

## Nearest Merger

~~~cpp
template<class DistanceMatrix, class DemandContainer>
vector<vector<int>> nearest_merger_routes(
    const DistanceMatrix& distance,
    const DemandContainer& demand,
    demand_value_t<DemandContainer> capacity,
    size_t depot = 0
)
~~~

単独顧客経路から開始し、容量内で併合可能な 2 経路と向きのうち、
depot 辺を接続辺へ置き換えた距離増分が最小の組を反復して併合する。
どの 2 経路も容量内で併合できなくなると終了する。

- 時間計算量: \(O(N^3)\)
- 追加空間計算量: \(O(N)\)

## 注意点

`distance` は正方行列でなければならない。容量制約付き API では
`demand.size() == distance.size()`、非負の `capacity` と需要、
`demand[depot] == 0`、各顧客需要が `capacity` 以下でなければならない。
これらへの違反、重複 depot、非空行列に対する空の depot 集合、
不正な `giant_order` は
`std::invalid_argument` を送出する。行列外の depot・頂点は
`std::out_of_range` を送出する。

`points` を受け取る API は行列と同じ個数の座標を要求し、違反時は
`std::invalid_argument` を送出する。頂点数が `int` で表現できない場合は
`std::length_error` を送出する。完全グラフから Hamilton path を構築できないという
内部不変条件違反は `std::logic_error` である。

距離要素型は既定構築、加算、減算、比較が可能で、全ての途中計算を表現できる必要がある。
需要要素型は既定構築、加算、減算、比較が可能でなければならない。
浮動小数点座標に NaN を含めてはならない。
各手法は heuristic であり、一般の距離行列に対する近似比を保証しない。
