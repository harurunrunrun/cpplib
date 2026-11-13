---
title: Integer Optimization Heuristics (整数最適化ヒューリスティクス)
documentation_of: ../../../src/approximate/optimization/integer_heuristics.hpp
---

0-1 変数・有界整数変数の最小化問題に対する、ソルバに依存しない主ヒューリスティクス（primal heuristic）群である。解は `vector<Int>` で表し、`feasible(x)` は制約を満たすかどうか、`objective(x)` は最小化する目的値を返す。各 API の時間計算量・空間計算量は以下で個別に示す。

以下、変数の数を $n$、反復回数を $I$、`feasible` 1 回のコストを $F$、`objective` 1 回のコストを $O$、近傍ソルバ 1 回のコストを $S$ とする。

## 共通のコールバックモデル

```cpp
enum class IntegerNeighborhoodPurpose {
    local_branching, rins, relax_and_fix, fix_and_optimize, proximity
};

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

近傍ソルバ `solve` はリクエストを受け取り、解があれば `optional<vector<Int>>` で返す。`fixed[i]` は固定値、`reference` からの距離は整数変数での値の不一致数（ハミング距離）、`objective_upper_bound` は厳密に改善すべき上限である。`purpose` により、ソルバ側で目的（特に proximity では距離を優先すること）を区別できる。

`IntegerHeuristicResult` は `solution`, `objective`, `iterations`, `candidates`, `feasible` を持つ。各メンバの参照と `operator bool()` は $O(1)$ である。特に断らない限り、リクエストの構築は $O(n)$ である。

## `local_branching`

```cpp
local_branching(
    incumbent, radius, solve, feasible, objective,
    maximum_iterations = 1
)
```

暫定解 `incumbent` からハミング距離 `radius` 以内で改善解を求め、改善したらその解を中心にして繰り返す。Fischetti–Lodi の local branching を近傍ソルバの呼び出しで表したものである。時間 $O(I(n+F+O+S))$、空間 $O(n)$（ソルバ内部を除く）である。

## `rins`

```cpp
rins(
    incumbent, relaxation, solve, feasible, objective,
    equality_tolerance = 1e-9L
)
```

暫定解と緩和解が一致する変数を固定し、残りを 1 回だけ再最適化する RINS である。時間 $O(n+F+O+S)$、空間 $O(n)$ である。

## `feasibility_pump`

```cpp
feasibility_pump(
    relaxation, lower, upper, feasible, objective, project,
    maximum_iterations
)
```

緩和解を丸めた整数点と、`project(integer_point)` が返す緩和問題への射影とを交互に求める。循環したら 1 変数を隣の整数値へ摂動する。連続な射影はコールバックとして呼び出し側が与える。時間 $O(I(n\log I+F+P)+O)$（$P$ は射影のコスト。既出の点は `set` で検出する）、空間 $O(In)$ である。

## `relax_and_fix`

```cpp
relax_and_fix(
    variable_count, blocks, solve, feasible, objective
)
```

ブロックの順に部分問題を解き、そのブロックで得られた整数値を以後固定する。固定済みのブロックが先頭から順に増えていく。ブロック数を $B$ として、時間 $O(B(n+S)+F+O)$、空間 $O(n)$ である。

## `fix_and_optimize`

```cpp
fix_and_optimize(
    incumbent, blocks, solve, feasible, objective, maximum_passes = 1
)
```

各ブロックだけを自由にし、それ以外を現在解に固定して改善することを、互いに素なブロックについて巡回的に繰り返す。パス数を $P$ として、時間 $O(PB(n+S+F+O))$、空間 $O(n)$ である。

## `diving_heuristic`

```cpp
diving_heuristic(
    lower, upper, solve_relaxation, feasible, objective,
    maximum_fixings = numeric_limits<size_t>::max(),
    integrality_tolerance = 1e-9L
)
```

`solve_relaxation(fixed)` を繰り返し呼び、まだ固定していない変数のうち整数に最も近い小数値の変数を丸めて固定する diving である。固定のたびに緩和問題を解き直す。固定回数を $K$、緩和問題 1 回のコストを $R$ として、時間 $O(K(n+R+F)+O)$、空間 $O(n)$ である。

## `rounding_heuristic`

```cpp
rounding_heuristic(
    relaxation, lower, upper, feasible, objective, repair
)
```

緩和解を最も近い整数へ丸め、実行不能な場合だけ `repair(rounded)` を 1 回呼ぶ。`repair` は `optional<vector<Int>>` を返す。修復のコストを $R$ として、時間 $O(n+2F+O+R)$、空間 $O(n)$ である。

## `proximity_search`

```cpp
proximity_search(
    incumbent, maximum_distance, solve, feasible, objective,
    maximum_iterations = 1
)
```

「現在値より厳密に良い解」という打ち切り条件の下で、現在解との距離を優先して最小化する部分問題を繰り返す。リクエストの `purpose` により、ソルバに距離の最小化を指示する。時間 $O(I(n+S+F+O))$、空間 $O(n)$ である。

## `solution_polishing`

```cpp
solution_polishing(
    incumbent, lower, upper, feasible, objective, maximum_passes = 1
)
```

ソルバを使わず、現在解から 1 変数を $\pm1$ する移動と 2 変数を同時に動かす移動をすべて評価し、パスごとに最良の改善を採用する決定的な手法である。パス数を $P$ として、時間 $O(Pn^2(n+F+O))$（候補ベクトルのコピーを含む）、空間 $O(n)$ である。

## 注意点

- すべての API は最小化用である。`objective_upper_bound` は厳密な改善を意味する
- `local_branching` と `rins` の暫定解は実行可能でなければならない
- ソルバのコールバックは `fixed`、距離、目的値の上限を守る責任がある。返された解が制約を満たすかはライブラリも `feasible` で確認するが、リクエストへの違反そのものは検査できない。返された解の長さが違う場合は例外を送出する
- `relax_and_fix` と `fix_and_optimize` のブロックは範囲内かつ互いに素でなければならない（範囲外は `std::out_of_range`、重複は `std::invalid_argument`）。空のブロックは許す
- 上下限を取る API では `Int` は整数型で、`lower <= upper` でなければならない。緩和解と射影は有限値でなければならない
- 次元の不一致や不正なパラメータには `std::invalid_argument` を送出する
- これらは主ヒューリスティクスであり、一般には最適性や実行可能解の発見を保証しない。現実的な数理計画問題では、コールバックのコストが支配的になる
