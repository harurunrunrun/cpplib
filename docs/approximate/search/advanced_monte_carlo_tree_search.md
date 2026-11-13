---
title: Advanced Monte Carlo Tree Search (高度なモンテカルロ木探索)
documentation_of: ../../../src/approximate/search/advanced_monte_carlo_tree_search.hpp
---

PUCT、RAVE/AMAF、MCTS-Solver、Progressive Widening、Double Progressive Widening、Progressive Bias と、木を保持しない Rollout、Nested Monte Carlo Search (NMCS)、Nested Rollout Policy Adaptation (NRPA) を提供する。

## 共通のゲーム API と結果

木探索 API は次のコールバックを受け取る。

- `terminal(state)` は終局なら真を返す。
- `legal_moves(state)` は合法手の範囲を返す。
- `transition(state, move)` は遷移後の新しい状態を返す。
- `player_to_move(state)` は等値比較可能な手番を返す。
- `reward(state, root_player)` は根プレイヤー視点の有限な報酬を返す。
- `random_engine` は `std::mt19937` などの一様乱数ビット生成器（UniformRandomBitGenerator）である。

```cpp
enum class MctsProvenOutcome { loss, draw, win, unknown };

template<class Move>
struct AdvancedMonteCarloTreeSearchResult {
    optional<Move> move;
    size_t visits;
    double average_reward;
    size_t tree_nodes;
    size_t amaf_updates;
    MctsProvenOutcome proven_outcome;
};
```

`move` は選択した根の手、`visits` と `average_reward` はその手の直接統計、`tree_nodes` は根を含む生成節点数である。`amaf_updates` は RAVE/AMAF で実際に行った手別更新数、`proven_outcome` は MCTS-Solver の根の証明値である。

## PUCT

```cpp
puct_monte_carlo_tree_search(
    root, iterations, terminal, legal_moves, transition,
    player_to_move, reward, prior, random_engine,
    exploration = 1, maximum_rollout_depth = 256);
```

`prior(state, move)` は非負有限値を返す。各節点で和が 1 になるよう正規化し、すべて 0 なら一様分布にする。選択値は、根の報酬方向へ符号を合わせた $Q(s,a)+cP(s,a)\sqrt{N(s)}/(1+N(s,a))$ である。未展開の手も事前確率が最大のものから展開するため、事前確率は展開順と選択の両方に使われる。

## RAVE と AMAF

```cpp
rave_monte_carlo_tree_search(
    root, iterations, terminal, legal_moves, transition,
    player_to_move, reward, random_engine,
    exploration = sqrt(2), rave_equivalence = 0.001,
    maximum_rollout_depth = 256);

amaf_monte_carlo_tree_search(
    root, iterations, terminal, legal_moves, transition,
    player_to_move, reward, random_engine,
    exploration = sqrt(2), maximum_rollout_depth = 256);
```

各節点は合法手ごとの AMAF 訪問数と報酬和を保持する。シミュレーション中でその節点以降に同じ手番のプレイヤーが初めて指した各手について、一度ずつ AMAF 統計を更新する。AMAF 版は AMAF 平均を、未観測の場合は直接の平均を使う。RAVE 版は直接平均と AMAF 平均を

$$
\beta=\frac{n_{\rm AMAF}}
 {n+n_{\rm AMAF}+4nn_{\rm AMAF}k^2}
$$

で混合する。$k$ が `rave_equivalence` である。

## MCTS-Solver

```cpp
mcts_solver(
    root, iterations, terminal, legal_moves, transition,
    player_to_move, reward, random_engine,
    exploration = sqrt(2), maximum_rollout_depth = 256);
```

終局報酬の符号を `win/draw/loss` とし、根手番の節点では「勝ちの子が一つあれば勝ち」、相手手番では「負けの子が一つあれば負け」を直ちに伝播する。全合法手が展開済みかつ全子が証明済みなら ミニマックス に従って draw または反対側の結果も証明する。根が証明されると反復を早期終了する。

## Progressive Widening と Progressive Bias

```cpp
struct MctsProgressiveWidening {
    double coefficient = 1;
    double exponent = 0.5;
};

progressive_widening_monte_carlo_tree_search(
    root, iterations, terminal, legal_moves, transition,
    player_to_move, reward, random_engine, widening = {},
    exploration = sqrt(2), maximum_rollout_depth = 256);

progressive_bias_monte_carlo_tree_search(
    root, iterations, terminal, legal_moves, transition,
    player_to_move, reward, bias, random_engine,
    bias_weight = 1, exploration = sqrt(2),
    maximum_rollout_depth = 256);
```

Progressive Widening は訪問数 $N$ の節点で展開済み子数を $\max(1,\lfloor kN^\alpha\rfloor)$ 以下に制限する。`bias(state, move)` は有限なヒューリスティック値で、Progressive Bias は UCT 値へ `bias_weight * bias / (1 + child_visits)` を加えるため、統計が増えると影響が消える。

## Double Progressive Widening

```cpp
double_progressive_widening_monte_carlo_tree_search(
    root, iterations, terminal, legal_moves, stochastic_transition,
    player_to_move, reward, random_engine,
    action_widening = {}, outcome_widening = {},
    exploration = sqrt(2), maximum_rollout_depth = 256);
```

`stochastic_transition(state, move, random_engine)` は確率遷移を 1 回標本化する。各状態の行動（action）の数を `action_widening` で、各行動から標本化した遷移結果（outcome）の数を `outcome_widening` で、それぞれ訪問数に依存する上限により制限する。遷移結果の数が上限に達した後は、既存の遷移結果（particle）をその訪問数に比例する確率で再利用する。

## 公開 Rollout

```cpp
monte_carlo_rollout(
    initial_state, terminal, legal_moves, transition, reward,
    reward_player, random_engine, maximum_depth = 256);

monte_carlo_rollout_with_policy(
    initial_state, terminal, legal_moves, transition, reward,
    reward_player, policy, random_engine, maximum_depth = 256);
```

`policy(state, moves, random_engine)` は `moves` の添字を返す。前者は一様方策を使う。返値 `MonteCarloRolloutResult<State, Move>` は `final_state`、実行した `moves`、`reward`、終局到達を示す `terminal` を持つ。

## Nested Monte Carlo Search

```cpp
nested_monte_carlo_search(
    root, level, terminal, legal_moves, transition, score,
    random_engine, maximum_depth = 256);
```

1 人用の最大化問題向けで、`score(state)` は有限値を返す。レベル 0 は一様なロールアウトである。レベル $l>0$ では、現在のすべての合法手についてレベル $l-1$ の探索を行って最良手を実行し、次の状態でも同じ入れ子の探索を繰り返す。`NestedMonteCarloSearchResult<Move>` は最良 `moves`、`score` と、実行したレベル 0 のロールアウト数 `rollouts` を返す。

## Nested Rollout Policy Adaptation / NRPA

```cpp
nested_rollout_policy_adaptation(
    root, level, iterations_per_level,
    terminal, legal_moves, transition, score,
    policy_index, policy_size, random_engine,
    learning_rate = 1, maximum_depth = 256);

nrpa(/* 同じ引数 */);
```

`policy_index(state, move)` は `[0, policy_size)` の範囲の logit の添字を返す。レベル 0 は softmax 方策でロールアウトする。上位のレベルは下位のレベルを `iterations_per_level` 回入れ子に呼び出し、それまでの最良の手順の各状態で選択した手の logit を上げ、全合法手の logit を softmax 確率に比例して下げる。したがって単なるロールアウトの反復ではなく、各レベルが独立な方策のコピーを適応させる。`nrpa` は完全な別名である。返値 `NrpaSearchResult<Move>` は最良の手順、スコア、最上位で適応させた `policy`、レベル 0 のロールアウト数を持つ。

## 時間計算量・空間計算量

合法手生成、遷移、報酬評価の 1 回の計算量を $G$, $T$, $E$、反復数を $I$、木の深さを $D$、ロールアウトの上限を $L$、最大展開済み分岐数を $b$ とする。

- PUCT、Progressive Widening、Progressive Bias は 1 反復あたり $O(D(b+G+T)+L(G+T)+E)$ で、全体はその $I$ 倍である。
- MCTS-Solver は証明伝播の子走査を含め同じ漸近上界である。
- RAVE/AMAF の AMAF 更新は、シミュレーションの長さを $H\le D+L$ とすると追加で 1 反復 $O(DH(b+H))$ である。
- DPW は行動の最大数を $a$、行動ごとの遷移結果の最大数を $o$ として 1 反復あたり $O(D(a+o+G+T)+L(G+T)+E)$ である。
- Rollout は時間 $O(L(G+T)+E)$、結果列を含む空間 $O(L)$ である。
- NMCS はレベル 0 のロールアウト数を正確に `rollouts` に返す。最大分岐数 $b$、レベル $\ell$ での粗い上界は $O((bD)^\ell D(G+T+E))$、再帰空間 $O(\ell D)$ である。
- NRPA のレベル 0 のロールアウト数は、$R$ を `iterations_per_level` として $R^\ell$ である。時間計算量は $O(R^\ell D(b+G+T+E))$、方策の長さを $P$ とした再帰空間は $O(\ell(P+D))$ である。

木探索の空間は、生成節点数 $N$ と保存した合法手総数 $M$ に対して $O(N+M)$。DPW では行動と遷移結果の節点もこの値に含む。

## 注意点

`Move` はコピー可能でなければならない。RAVE/AMAF と NRPA ではさらに `operator==` が必要である。幅制御の `coefficient` は正の有限値、`exponent` は $[0,1]$ の有限値でなければならない。探索係数、`bias_weight`、NRPA の `learning_rate` は非負の有限値でなければならない。これらの違反には `std::invalid_argument` を、方策が範囲外の添字を返した場合は `std::out_of_range` を、PUCT の事前確率の和が有限でない場合は `std::overflow_error` を送出する。

深さ上限で非終局の状態にも `reward` / `score` を定義する必要がある。MCTS-Solver は報酬の符号だけを勝敗証明に使う。DPW の遷移結果は同じ状態どうしを同一視せず、確率分布の標本（particle）として保持する。NRPA で異なる状態・行動の組に同じ添字を割り当てると、それらは同じ logit を共有して学習する。

乱数エンジンの状態と合法手の順序を固定すれば、結果は再現できる。
