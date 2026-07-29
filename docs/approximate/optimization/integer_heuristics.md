---
title: Integer Optimization Heuristics (整数最適化ヒューリスティクス)
documentation_of: ../../../src/approximate/optimization/integer_heuristics.hpp
---

binary/integer 最小化問題向けの solver-independent な primal heuristic 群。
解は `vector<Int>`、`feasible(x)` は制約充足、`objective(x)` は最小化目的値を
返す。This header supplies solver-independent primal heuristics for binary
and bounded-integer minimization.
各 API の時間計算量は以下で個別に示す。

## 共通 callback model / Common callback model

```cpp
enum class IntegerNeighborhoodPurpose;

template<class Int, class Value>
struct IntegerNeighborhoodRequest {
    vector<optional<Int>> fixed;
    vector<Int> reference;
    size_t maximum_distance;
    optional<Value> objective_upper_bound;
    IntegerNeighborhoodPurpose purpose;
};

template<class Int, class Value> struct IntegerHeuristicResult;
```

neighborhood solver は request を受け、解があれば
`optional<vector<Int>>` を返す。`fixed[i]` は固定値、`reference` からの距離は
整数変数では値の不一致数（Hamming 距離）、`objective_upper_bound` は厳密に
改善すべき上限である。`purpose` により solver 側で目的（特に proximity
では距離優先）を区別できる。

`IntegerHeuristicResult` は `solution`, `objective`, `iterations`,
`candidates`, `feasible` を持つ。各メンバ参照と `operator bool()` は $O(1)$。
Request construction is $O(n)$ unless noted; a solver call has user-defined
cost $S(n)$.

## `local_branching`

```cpp
local_branching(
    incumbent, radius, solve, feasible, objective,
    maximum_iterations = 1
)
```

incumbent の Hamming 半径 `radius` 内で改善解を求め、改善した中心から反復する。
This is Fischetti-Lodi local branching expressed through neighborhood solves.
時間 $O(I(n+F+O+S))$、空間 $O(n)$（solver 内部を除く）。

## `rins`

```cpp
rins(
    incumbent, relaxation, solve, feasible, objective,
    equality_tolerance = 1e-9L
)
```

incumbent と relaxation が一致する変数を固定し、残りを一度再最適化する RINS。
It fixes agreement variables and makes one subproblem call.
時間 $O(n+F+O+S)$、空間 $O(n)$。

## `feasibility_pump`

```cpp
feasibility_pump(
    relaxation, lower, upper, feasible, objective, project,
    maximum_iterations
)
```

丸めた整数点と、`project(integer_point)` が返す relaxation への射影を交互に行う。
循環時は一変数を隣の整数値へ摂動する。This is feasibility-pump control
with a caller-supplied continuous projection callback.
時間 $O(I(n\log I+F+P)+O)$（既出点を `set` で検出）、空間 $O(In)$。

## `relax_and_fix`

```cpp
relax_and_fix(
    variable_count, blocks, solve, feasible, objective
)
```

block 順に部分問題を解き、その block の得られた整数値を以後固定する。
Relax-and-fix progressively grows the fixed prefix of blocks.
block 数 $B$ に対し時間 $O(B(n+S)+F+O)$、空間 $O(n)$。

## `fix_and_optimize`

```cpp
fix_and_optimize(
    incumbent, blocks, solve, feasible, objective, maximum_passes = 1
)
```

各 block だけを自由にし、外側を現在解へ固定して改善を反復する。
Fix-and-optimize cycles through disjoint free-variable blocks.
時間 $O(PB(n+S+F+O))$、空間 $O(n)$。

## `diving_heuristic`

```cpp
diving_heuristic(
    lower, upper, solve_relaxation, feasible, objective,
    maximum_fixings = size_t max, integrality_tolerance = 1e-9L
)
```

`solve_relaxation(fixed)` を反復し、まだ固定していない中で整数に最も近い
fractional 変数を丸めて固定する diving。At every dive it re-solves the
relaxation and fixes the least-fractional unfixed variable.
固定回数 $K$ に対し時間 $O(K(n+R+F)+O)$、空間 $O(n)$。

## `rounding_heuristic`

```cpp
rounding_heuristic(
    relaxation, lower, upper, feasible, objective, repair
)
```

最近整数へ丸め、失敗時だけ `repair(rounded)` を一度呼ぶ。The repair callback
returns `optional<vector<Int>>`. 時間 $O(n+2F+O+R)$、空間 $O(n)$。

## `proximity_search`

```cpp
proximity_search(
    incumbent, maximum_distance, solve, feasible, objective,
    maximum_iterations = 1
)
```

現在値より厳密に良い解という cutoff の下で、現在解との距離を優先する部分問題を
反復する。The request purpose tells the solver to minimize proximity under
the improvement cutoff. 時間 $O(I(n+S+F+O))$、空間 $O(n)$。

## `solution_polishing`

```cpp
solution_polishing(
    incumbent, lower, upper, feasible, objective, maximum_passes = 1
)
```

solver を使わず、現在解から $\pm1$ の一変数 move と二変数 move を全評価し、
pass ごとの best improvement を採用する。
It performs deterministic one- and two-coordinate polishing.
変数数 $n$、pass 数 $P$ に対し時間 $O(Pn^2(n+F+O))$
（候補 vector の copy を含む）、空間 $O(n)$。

## 注意点

Notes:

- 全 API は最小化用。`objective_upper_bound` は strict improvement を意味する。
- solver callback は `fixed`、距離、目的上限を守る責任がある。返却解の制約充足は
  ライブラリも `feasible` で確認するが、request 違反そのものは検査できない。
- `relax_and_fix` と `fix_and_optimize` の block は範囲内かつ互いに素である必要が
  ある。空 block は許される。
- bounds を取る API は `Int` が整数型で、`lower <= upper` を要求する。
  relaxation と projection は有限値でなければならない。
- これらは primal heuristic であり、最適性や実行可能解発見を一般には保証しない。
  Callback costs dominate on realistic mathematical programs.
