---
title: Adaptive Constructive Metaheuristics (適応的構築メタヒューリスティクス)
documentation_of: ../../../src/approximate/metaheuristic/adaptive_constructive_search.hpp
---

タブー期間（tenure）、GRASP のパラメータ、破壊・修復演算子、分布のパラメータなどを探索中に制御する汎用の最小化 API である。問題の表現はコールバックで定義し、ライブラリはメタヒューリスティクスの制御ロジックだけを受け持つ。

以下、反復回数を $I$、評価 1 回のコストを $E$ とする。

## `reactive_tabu_search`

```cpp
reactive_tabu_search(
    initial, neighborhood, evaluate, key, iterations,
    initial_tenure, minimum_tenure, maximum_tenure,
    decrease_interval, hash = {}, compare = {}
)
```

解の再訪を検出するとタブー期間を 1 増やし、サイクルが起きないまま `decrease_interval` 反復が経過するごとに 1 減らす Reactive Tabu Search である。全体の最良解を改善するタブーな遷移には特例（aspiration）を適用する。$1\le \texttt{minimum_tenure} \le \texttt{initial_tenure} \le \texttt{maximum_tenure}$ かつ `decrease_interval` が正でなければならない。

反復 $t$ の近傍数を $N_t$ とすると、期待時間計算量は $O(E+\sum_tN_tE)$、追加空間計算量は $O(I+\max_tN_t)$ である。ハッシュの最悪時は $O(I\sum_tN_t)$ になる。

## `granular_tabu_search`

```cpp
granular_tabu_search(
    initial, neighborhood, evaluate, key, granular_filter,
    iterations, tabu_tenure, hash = {}, compare = {}
)
```

`granular_filter(current, candidate)` が真になる疎な近傍だけを評価してタブー探索を行う。`tabu_tenure` は正でなければならない。フィルタ 1 回のコストを $F$、反復 $t$ でフィルタを通過した候補数を $A_t$ とすると、期待時間計算量は $O(\sum_tN_tF+E\sum_tA_t)$、追加空間計算量は $O(I)$ である。

## `reactive_grasp`

```cpp
reactive_grasp(
    construct, local_improve, evaluate, alpha_values,
    iterations, adaptation_interval, rng, compare = {}
)
```

`construct(alpha, rng)` に渡す `alpha` をルーレット選択し、区間ごとの平均目的値と全体の最良値との差の逆数で選択重みを更新する Reactive GRASP である。各 `alpha` は $[0,1]$、`alpha_values` は空でなく、`iterations` と `adaptation_interval` は正でなければならない。

`alpha` の個数を $A$、構築のコストを $C$、局所改善のコストを $L$ とすると、時間計算量は $O(I(C+L+E+A))$、追加空間計算量は $O(A)$ である。

## `large_neighborhood_search`

```cpp
large_neighborhood_search(
    initial, destroy, repair, evaluate, accept,
    iterations, rng, compare = {}
)
```

各反復で破壊（destroy）と修復（repair）を行い、`accept(candidate_score, current_score, iteration, rng)` が真なら候補を現在解にする大近傍探索（LNS）である。受理判定を明示的なコールバックにすることで、改善時のみ受理、焼きなまし、record-to-record などを選べる。

破壊・修復・受理判定のコストを $D, R, A$ とすると、時間計算量は $O(E+I(D+R+E+A))$、追加空間計算量は部分解と状態が各 1 個分である。

## `adaptive_large_neighborhood_search`

```cpp
adaptive_large_neighborhood_search(
    initial, destroys, repairs, evaluate, iterations,
    reaction_factor, rng, compare = {}
)
```

破壊演算子と修復演算子を独立にルーレット選択し、全体の最良解を更新したら 5、現在解を改善して受理したら 2、不採用なら 0.1 の報酬を与えて指数平滑化で重みを更新する ALNS である。候補は現在解より良いときだけ受理する。演算子の集合は空でなく、`reaction_factor` は有限で $(0,1]$ になければならない。

演算子の個数を $D_o, R_o$ とすると、時間計算量は $O(E+I(D_o+R_o+D+R+E))$、追加空間計算量は $O(D_o+R_o)$ である。

## `ruin_and_recreate`

```cpp
ruin_and_recreate(
    initial, ruin, recreate, evaluate, iterations,
    initial_ruin_size, maximum_ruin_size, rng, compare = {}
)
```

改善しなかった反復ごとに破壊サイズを 1 増やし（上限 `maximum_ruin_size`）、改善したら初期値に戻す Ruin and Recreate である。$1\le \texttt{initial_ruin_size} \le \texttt{maximum_ruin_size}$ でなければならない。破壊・再構築のコストを $U, R$ とすると、時間計算量は $O(E+I(U+R+E))$、追加空間計算量は部分解と状態が各 1 個分である。

## `scatter_search`

```cpp
scatter_search(
    reference_set, combine, local_improve, evaluate,
    iterations, reference_set_size, rng, compare = {}
)
```

参照集合の順序を問わないすべての対を結合して局所改善し、親と子を合わせた集団の上位を次の参照集合とする Scatter Search である。入力の参照集合は空でなく、保持数 `reference_set_size` は正でなければならない。

保持数を $N$、結合のコストを $C$、局所改善のコストを $L$ とすると、時間計算量は $O(NE+I(N^2(C+L+E)+N^2\log N))$、追加空間計算量は $O(N^2)$ 個の状態である。

## `pilot_method`

```cpp
pilot_method(
    initial_partial, complete, extensions, rollout,
    evaluate, maximum_steps, compare = {}
)
```

構築の各ステップで、すべての拡張候補をロールアウトして評価し、最良のロールアウトを与える拡張を確定する Pilot Method である。初期の部分解も 1 回ロールアウトする。ステップ $t$ の拡張候補数を $B_t$、ロールアウトのコストを $R$ とすると、時間計算量は $O(R+E+\sum_tB_t(R+E))$、追加空間計算量は $O(\max_tB_t)$ 個の状態である。

## `cross_entropy_method`

```cpp
cross_entropy_method(
    parameters, sample, evaluate, update,
    iterations, population_size, elite_count, rng, compare = {}
)
```

各反復で `sample(parameters, rng)` を `population_size` 回呼び、上位 `elite_count` 個を `update(elites, parameters)` に渡して、その返り値を新しい分布パラメータとする Cross-Entropy Method である。`iterations`, `population_size`, `elite_count` は正で、`elite_count <= population_size` でなければならない。

集団サイズを $P$、標本生成のコストを $S$、更新のコストを $U$ とすると、時間計算量は $O(I(P(S+E)+P\log P+U))$、追加空間計算量は $O(P)$ 個の状態と目的値である。

## 共通操作の計算量

各 API が返す `SearchResult` のメンバの参照は時間・追加空間とも $O(1)$ である。

## 注意点

- パラメータが上記の条件を満たさない場合は `std::invalid_argument` を送出する
- デフォルトは最小化である。Cross-Entropy、Scatter Search、GRASP は `Compare` を一貫して使う
- ALNS の破壊演算子はすべて同じ部分解の型を返し、すべての修復演算子がその型を受け取る呼び出し可能オブジェクトでなければならない
- タブー探索のキーの `Hash` と等値関係、参照集合やコールバックの寿命は、呼び出し側が一貫して管理する
- 評価回数には、ロールアウトや局所改善などのユーザーコールバック内で行われた評価は含まれない
- いずれも有限時間での最適性は保証しない
