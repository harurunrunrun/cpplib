---
title: Incremental D-Star and LPA Search (増分Dスター・LPA探索)
documentation_of: ../../../src/approximate/search/incremental_path_search.hpp
---

正の重みを持つ可変有向グラフに対する D*、Focused D*、D* Lite、Lifelong Planning A* (LPA*) と、動的障害物グリッド用 Field D* を提供する。各プランナは $g$ と 1 段先読みした値 $\mathit{rhs}$ を保持し、辺の変更後に不整合になった頂点だけを再処理する。毎回探索全体をやり直す API ではない。

```cpp
template<class Cost>
struct IncrementalPathResult {
    optional<Cost> cost;
    vector<int> path;
    size_t expanded;
};
```

頂点は `[0, vertex_count)` の整数、辺は有向である。`Cost` は全順序、加算、デフォルト構築を持つ数値型とし、辺のコストは真に正でなければならない。正という制約により、経路復元時にコスト 0 の閉路が生じることを防ぐ。到達できない場合は `cost == nullopt` となる。頂点数を $V$、辺数を $E$ とする。

## Lifelong Planning A* / LPA*

```cpp
LifelongPlanningAStar<Cost> planner(
    vertex_count, start, goal, heuristic = {}, infinity = default);
LPAStar<Cost> alias(...);

planner.set_edge(from, to, cost);
planner.remove_edge(from, to);
planner.replan();
planner.g_value(vertex);
```

始点から前向きに $\mathit{rhs}(v)=\min_u(g(u)+c(u,v))$ を保ち、キー $(\min(g,\mathit{rhs})+h(v,\mathrm{goal}), \min(g,\mathit{rhs}))$ の順に不整合な頂点を処理する LPA* である。`LPAStar` は同じ Lifelong Planning A* の型の別名であり、別のアルゴリズムではない。

`set_edge` / `remove_edge` は、ハッシュ表の更新が期待 $O(1)$、変更先の $\mathit{rhs}$ の再計算が入次数 $d^-(v)$ に対して $O(d^-(v)+\log Q)$。`g_value` は $O(1)$。`replan` で展開した頂点数を $X$、走査した辺数を $A$、OPEN の最大サイズを $Q$ とすると、時間計算量は $O(A+X\log Q)$、保持する空間全体は $O(V+E+Q)$。

## D*

```cpp
DStar<Cost> planner(vertex_count, start, goal, infinity = default);
planner.set_edge(from, to, cost);
planner.remove_edge(from, to);
planner.move_start(new_start);
planner.replan();
```

目標から後ろ向きに残りコスト（cost-to-go）を保持する動的な D* である。ヒューリスティックは使わず、$g>\mathit{rhs}$ の lower 状態と $g \le \mathit{rhs}$ の raise 状態を伝播する。辺を変更すると変更元の $\mathit{rhs}$ が不整合になる。`move_start` は $O(1)$ で、未処理の OPEN は次の `replan` で必要な範囲まで処理する。

辺 1 本の更新は、変更元の出次数 $d^+(u)$ に対して $O(d^+(u)+\log Q)$。`replan` は $O(A+X\log Q)$、空間計算量は $O(V+E+Q)$。

## Focused D*

```cpp
FocusedDStar<Cost> planner(
    vertex_count, start, goal, heuristic, infinity = default);
```

D* の後ろ向きの修復に $h(\mathrm{start},v)$ を加え、現在位置に関係する頂点へ処理を集中させる。ヒューリスティックが 0 なら D* と同じ距離優先になるが、キーの規則は異なる。開始点を変更すると全不整合頂点のキーを再構築するため `move_start` は $O(V+Q)$ で、辺の更新と `replan` の計算量は D* と同じ。

## D* Lite

```cpp
DStarLite<Cost> planner(
    vertex_count, start, goal, heuristic, infinity = default);
```

後ろ向きに $g$ / $\mathit{rhs}$ の修復を行い、キーに $h(\mathrm{start},v)+k_m$ を使う。`move_start` は $k_m$ に $h(\mathrm{old\ start},\mathrm{new\ start})$ を加えるだけなので、OPEN の再構築が不要で $O(1)$。辺の更新は $O(d^+(u)+\log Q)$、`replan` は $O(A+X\log Q)$、空間計算量は $O(V+E+Q)$。

## Field D*

```cpp
FieldDStar planner(grid, start, goal);
planner.grid();
planner.set_passable(point, passable);
planner.move_start(new_start);
planner.replan();
```

8 近傍の動的な残りコストの場（field）を D* Lite で修復し、対角移動では両隣のセルが通行可能であることを要求する。復元した離散経路について、場の上で見通し（LOS）の通る最も遠い点へ線形補間するため、障害物を横切らない任意角の `field_path` も返す。

```cpp
struct FieldDStarResult {
    optional<double> discrete_cost;
    optional<double> interpolated_cost;
    vector<GridPoint> grid_path;
    vector<GridPoint> field_path;
    size_t expanded;
};
```

グリッドの行数を $H$、列数を $W$ とすると、構築の時間計算量・空間計算量は $O(HW)$。`set_passable` は変更したセルを含む定数個の局所的な辺だけを更新し、期待 $O(\log Q)$（グリッドの次数は最大 8）。`move_start` は $O(1)$。`replan` の修復は $O(A+X\log Q)$、経路の補間は復元経路の長さを $P$、LOS が調べるセルの総数を $L$ として $O(P+L)$。

## 時間計算量

各公開 API の時間計算量・空間計算量は、上の各 API の節に個別に記載している。

## 注意点

- すべてのプランナは、負の辺とコスト 0 の辺を拒否する（`std::invalid_argument`）。
- 負の `vertex_count`、正でない `infinity`、Focused D* / D* Lite でヒューリスティックが空の場合も `std::invalid_argument` を送出する。範囲外の頂点や、Field D* でグリッド外の端点・空のグリッドには `std::out_of_range` を送出する。
- `infinity` は、すべての有限な経路コストや `g + edge + heuristic` より大きくする。
- ヒューリスティックは非負かつ整合的でなければならない。常に 0 を返すものは安全である。
- 有向辺の逆向きは自動では追加されない。無向辺は両方向を更新する。
- `set_edge` は新規追加と重みの変更の両方に使える。`remove_edge` は存在しない辺に対しても安全である。
- Field D* の `discrete_cost` は 8 近傍での最短距離、`interpolated_cost` は LOS 補間後の経路長であり、通常は後者が前者以下になる。
