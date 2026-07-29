---
title: Incremental D-Star and LPA Search (増分Dスター・LPA探索)
documentation_of: ../../../src/approximate/search/incremental_path_search.hpp
---

## 日本語

正の重みを持つ可変有向グラフに対する D*、Focused D*、D* Lite、
Lifelong Planning A* (LPA*) と、動的障害物グリッド用 Field D* を提供する。
各 planner は `g` と一段先読み値 `rhs` を保持し、辺変更後に不整合になった
頂点だけを再処理する。全探索を毎回やり直す API ではない。

```cpp
template<class Cost>
struct IncrementalPathResult {
    optional<Cost> cost;
    vector<int> path;
    size_t expanded;
};
```

頂点は `[0, vertex_count)` の整数、辺は有向である。`Cost` は全順序、加算、
既定構築を持つ数値型とし、辺コストは厳密に正でなければならない。正の制約は
経路復元時のゼロコスト cycle を排除する。未到達なら `cost == nullopt`。

### Lifelong Planning A* / LPA*

```cpp
LifelongPlanningAStar<Cost> planner(
    vertex_count, start, goal, heuristic = {}, infinity = default);
LPAStar<Cost> alias(...);

planner.set_edge(from, to, cost);
planner.remove_edge(from, to);
planner.replan();
planner.g_value(vertex);
```

始点から前向きに `rhs(v)=min(g(u)+c(u,v))` を保ち、key
`(min(g,rhs)+h(v,goal), min(g,rhs))` で不整合頂点を処理する LPA*。
`LPAStar` は同じ正式名称 Lifelong Planning A* の型別名であり、別アルゴリズム
ではない。

`set_edge`/`remove_edge` はハッシュ表更新が期待 \(O(1)\)、変更先の
`rhs` 再計算が入次数 \(d^-(v)\) に対して \(O(d^-(v)+\log Q)\)。
`g_value` は \(O(1)\)。`replan` で展開した頂点数を \(X\)、走査した辺数を
\(A\)、最大 OPEN サイズを \(Q\) とすると時間 \(O(A+X\log Q)\)、
全保持空間は \(O(V+E+Q)\)。

### D*

```cpp
DStar<Cost> planner(vertex_count, start, goal, infinity = default);
planner.set_edge(from, to, cost);
planner.remove_edge(from, to);
planner.move_start(new_start);
planner.replan();
```

目標から後向きに cost-to-go を保持する動的 D*。ヒューリスティックを使わず、
`g>rhs` の lower と `g<=rhs` の raise を伝播する。辺変更は変更元の `rhs` を
不整合化する。`move_start` は \(O(1)\) で、未処理の OPEN frontier を次の
`replan` が必要な範囲まで進める。

辺一回の更新は変更元の出次数 \(d^+(u)\) に対し
\(O(d^+(u)+\log Q)\)。`replan` は \(O(A+X\log Q)\)、空間
\(O(V+E+Q)\)。

### Focused D*

```cpp
FocusedDStar<Cost> planner(
    vertex_count, start, goal, heuristic, infinity = default);
```

D* の後向き repair に `h(start,v)` を加え、現在位置に関係する頂点へ処理を
focus する。ゼロヒューリスティックなら D* と同じ距離優先になるが、別の
key 規則を持つ。開始点変更時は全不整合頂点の key を再構築するため
`move_start` は \(O(V+Q)\)、辺更新と `replan` は D* と同じ漸近量。

### D* Lite

```cpp
DStarLite<Cost> planner(
    vertex_count, start, goal, heuristic, infinity = default);
```

後向き `g/rhs` repair を行い、key に
`h(start,v)+km` を使う。`move_start` は
`km += h(old_start,new_start)` とするため OPEN の全再構築が不要で \(O(1)\)。
辺更新は \(O(d^+(u)+\log Q)\)、`replan` は \(O(A+X\log Q)\)、
空間は \(O(V+E+Q)\)。

### Field D*

```cpp
FieldDStar planner(grid, start, goal);
planner.grid();
planner.set_passable(point, passable);
planner.move_start(new_start);
planner.replan();
```

8 近傍の動的 cost-to-go field を D* Lite で修復し、対角移動では両隣接セルが
通行可能であることを要求する。復元した離散経路に対して、field 上で LOS の
通る最遠点へ線形補間するため、障害物を横切らない任意角の `field_path` も返す。

```cpp
struct FieldDStarResult {
    optional<double> discrete_cost;
    optional<double> interpolated_cost;
    vector<GridPoint> grid_path;
    vector<GridPoint> field_path;
    size_t expanded;
};
```

構築は \(O(HW)\) 時間・空間。`set_passable` は変更セルを含む定数個の
局所辺だけを更新し、期待 \(O(\log Q)\)（グリッド次数は最大 8）。
`move_start` は \(O(1)\)。`replan` の repair は
\(O(A+X\log Q)\)、経路補間は復元経路長を \(P\)、LOS が調べるセル総数を
\(L\) として \(O(P+L)\)。

### 時間計算量

各公開APIの時間・空間計算量は、上の各API節に個別に記載している。

### 注意点

- 全 planner は負辺とゼロ辺を拒否する。
- `infinity` は全有限経路コストと `g + edge + heuristic` より大きくする。
- ヒューリスティックは非負かつ整合的である必要がある。ゼロは常に安全。
- 有向辺の逆向きは自動追加されない。無向辺は両方向を更新する。
- `set_edge` は新規追加と重み変更の両方、`remove_edge` は存在しない辺にも安全。
- Field D* の `discrete_cost` は 8 近傍最短距離、
  `interpolated_cost` は LOS 補間後の経路長であり、通常は前者以下になる。

## English

The header supplies incremental planners for a mutable, strictly
positive-weight directed graph. Every planner retains `g`, one-step lookahead
`rhs`, and a lazy priority queue between calls; an edge update therefore repairs
only inconsistent states.

`LifelongPlanningAStar<Cost>` is the forward LPA* algorithm.
`LPAStar<Cost>` is its synonymous type alias. It uses
`(min(g,rhs)+h(v,goal), min(g,rhs))`. An edge update costs expected
\(O(d^-(v)+\log Q)\), while a repair with \(X\) expansions and \(A\) scanned
arcs costs \(O(A+X\log Q)\).

`DStar<Cost>` performs an unfocused backward raise/lower repair and changes its
start in \(O(1)\). `FocusedDStar<Cost>` adds `h(start,v)` to the backward key;
changing its start rebuilds all inconsistent keys in \(O(V+Q)\).
`DStarLite<Cost>` instead maintains the standard `km` start-motion correction,
so `move_start` is \(O(1)\) without rebuilding OPEN. Their edge updates cost
\(O(d^+(u)+\log Q)\), repairs cost \(O(A+X\log Q)\), and retained space is
\(O(V+E+Q)\).

`FieldDStar` maintains an eight-neighbor, no-corner-cut grid field with D* Lite.
`set_passable` changes only a constant-size local edge neighborhood.
`replan` returns both the exact discrete grid cost/path and a collision-free
line-of-sight interpolated field path. Construction takes \(O(HW)\);
incremental repair takes \(O(A+X\log Q)\), followed by path extraction and LOS
work.

All graph costs must be strictly positive. Heuristics must be non-negative and
consistent; zero is always valid. Directed reverse arcs are not inserted
automatically, and `infinity` must exceed every representable finite path/key
used by the application.
