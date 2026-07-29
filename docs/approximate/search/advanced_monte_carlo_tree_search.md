---
title: Advanced Monte Carlo Tree Search (高度なモンテカルロ木探索)
documentation_of: ../../../src/approximate/search/advanced_monte_carlo_tree_search.hpp
---

## 日本語

PUCT、RAVE/AMAF、MCTS-Solver、Progressive Widening、Double Progressive
Widening、Progressive Bias と、木を保持しない Rollout、Nested Monte Carlo
Search (NMCS)、Nested Rollout Policy Adaptation (NRPA) を提供する。

### 共通のゲーム API と結果

木探索 API は次の callback を受け取る。

- `terminal(state)` は終局なら真を返す。
- `legal_moves(state)` は合法手の range を返す。
- `transition(state, move)` は遷移後の新しい状態を返す。
- `player_to_move(state)` は等値比較可能な手番を返す。
- `reward(state, root_player)` は根プレイヤー視点の有限な報酬を返す。
- `random_engine` は `std::mt19937` などの UniformRandomBitGenerator である。

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

`move` は選択した根の手、`visits` と `average_reward` はその手の直接統計、
`tree_nodes` は根を含む生成節点数である。`amaf_updates` は RAVE/AMAF
で実際に行った手別更新数、`proven_outcome` は MCTS-Solver の根の証明値である。

### PUCT

```cpp
puct_monte_carlo_tree_search(
    root, iterations, terminal, legal_moves, transition,
    player_to_move, reward, prior, random_engine,
    exploration = 1, maximum_rollout_depth = 256);
```

`prior(state, move)` は非負有限値を返す。各節点で和が 1 になるよう正規化し、
全て 0 なら一様分布にする。選択値は、根の報酬方向へ符号を合わせた
$Q(s,a)+cP(s,a)\sqrt{N(s)}/(1+N(s,a))$ である。未展開手も prior 最大のものから
展開するため、prior は展開順と選択の両方へ実際に使われる。

### RAVE と AMAF

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

各節点は合法手ごとの AMAF 訪問数と報酬和を保持する。simulation のその節点
以降に同じ手番が初めて指した各手を一度ずつ AMAF 更新する。AMAF 版は AMAF
平均を、未観測時は直接平均を使用する。RAVE 版は直接平均と AMAF 平均を

$$
\beta=\frac{n_{\rm AMAF}}
 {n+n_{\rm AMAF}+4nn_{\rm AMAF}k^2}
$$

で混合する。`k` が `rave_equivalence` である。

### MCTS-Solver

```cpp
mcts_solver(
    root, iterations, terminal, legal_moves, transition,
    player_to_move, reward, random_engine,
    exploration = sqrt(2), maximum_rollout_depth = 256);
```

終局報酬の符号を `win/draw/loss` とし、根手番の節点では「勝ちの子が一つ
あれば勝ち」、相手手番では「負けの子が一つあれば負け」を直ちに伝播する。
全合法手が展開済みかつ全子が証明済みなら minimax に従って draw または
反対側の結果も証明する。根が証明されると反復を早期終了する。

### Progressive Widening と Progressive Bias

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

Progressive Widening は訪問数 $N$ の節点で展開済み子数を
$\max(1,\lfloor kN^\alpha\rfloor)$ 以下に制限する。`bias(state, move)` は有限な
heuristic 値で、Progressive Bias は UCT 値へ
`bias_weight * bias / (1 + child_visits)` を加えるため、統計が増えると影響が
消える。

### Double Progressive Widening

```cpp
double_progressive_widening_monte_carlo_tree_search(
    root, iterations, terminal, legal_moves, stochastic_transition,
    player_to_move, reward, random_engine,
    action_widening = {}, outcome_widening = {},
    exploration = sqrt(2), maximum_rollout_depth = 256);
```

`stochastic_transition(state, move, random_engine)` は確率遷移を 1 回標本化する。
各状態の action 数を `action_widening`、各 action の標本 outcome 数を
`outcome_widening` の訪問数依存上限で別々に制限する。outcome 上限到達後は
既存の outcome particle をその訪問数に比例して再利用する。

### 公開 Rollout

```cpp
monte_carlo_rollout(
    initial_state, terminal, legal_moves, transition, reward,
    reward_player, random_engine, maximum_depth = 256);

monte_carlo_rollout_with_policy(
    initial_state, terminal, legal_moves, transition, reward,
    reward_player, policy, random_engine, maximum_depth = 256);
```

`policy(state, moves, random_engine)` は `moves` の添字を返す。前者は一様方策を
使う。返り値 `MonteCarloRolloutResult<State, Move>` は `final_state`、
実行した `moves`、`reward`、終局到達を示す `terminal` を持つ。

### Nested Monte Carlo Search

```cpp
nested_monte_carlo_search(
    root, level, terminal, legal_moves, transition, score,
    random_engine, maximum_depth = 256);
```

単一プレイヤー最大化問題用で、`score(state)` は有限値を返す。level 0 は一様
rollout である。level $l>0$ は現在の全合法手から level $l-1$ の探索を行って
最良手を実行し、次の状態でも同じ nesting を繰り返す。
`NestedMonteCarloSearchResult<Move>` は最良 `moves`、`score` と、実行した
level 0 `rollouts` 数を返す。

### Nested Rollout Policy Adaptation / NRPA

```cpp
nested_rollout_policy_adaptation(
    root, level, iterations_per_level,
    terminal, legal_moves, transition, score,
    policy_index, policy_size, random_engine,
    learning_rate = 1, maximum_depth = 256);

nrpa(/* 同じ引数 / same arguments */);
```

`policy_index(state, move)` は `[0, policy_size)` の logit 添字を返す。level 0
は softmax 方策で rollout する。上位 level は下位 level を
`iterations_per_level` 回入れ子に呼び、best-so-far sequence の各状態で
選択手の logit を上げ、全合法手を softmax 確率に比例して下げる。したがって
単なる反復 rollout ではなく、各 level が独立な policy copy を適応する。
`nrpa` は完全な別名である。返り値 `NrpaSearchResult<Move>` は最良列、score、
最上位で適応済みの `policy`、level 0 rollout 数を持つ。

### 時間計算量・空間計算量

合法手生成、遷移、報酬評価の 1 回の計算量を $G,T,E$、反復数を $I$、木深さを
$D$、rollout 上限を $L$、最大展開済み分岐数を $b$ とする。

- PUCT、Progressive Widening、Progressive Bias は 1 反復
  $O(D(b+G+T)+L(G+T)+E)$、全体 $O(I)$ 倍である。
- MCTS-Solver は証明伝播の子走査を含め同じ漸近上界である。
- RAVE/AMAF の AMAF 更新は、simulation 長を $H\le D+L$ とすると追加で
  1 反復 $O(DH(b+H))$ である。
- DPW は最大 action particle 数を $a$、action ごとの outcome particle 数を
  $o$ として 1 反復 $O(D(a+o+G+T)+L(G+T)+E)$ である。
- Rollout は時間 $O(L(G+T)+E)$、結果列を含む空間 $O(L)$ である。
- NMCS は level 0 rollout 数を正確に `rollouts` へ返す。最大分岐 $b$、
  level $\ell$ の粗い上界は $O((bD)^\ell D(G+T+E))$、再帰空間
  $O(\ell D)$ である。
- NRPA の level 0 rollout 数は $R^\ell$（`R = iterations_per_level`）。
  時間は $O(R^\ell D(b+G+T+E))$、policy 長を $P$ とした再帰空間は
  $O(\ell(P+D))$ である。

木探索の空間は、生成節点数 $N$ と保存した合法手総数 $M$ に対して
$O(N+M)$。DPW は action/outcome particle もこの値へ含む。

### 注意点

`Move` はコピー可能でなければならない。RAVE/AMAF と NRPA ではさらに
`operator==` が必要である。幅制御の `coefficient` は正の有限値、
`exponent` は有限な `[0,1]` でなければならない。探索係数、bias weight、
NRPA learning rate は非負有限値でなければならない。

深さ上限で非終局の状態にも `reward` / `score` を定義する必要がある。
MCTS-Solver は報酬の符号だけを勝敗証明に使う。DPW の outcome は同一状態を
同一視せず、確率分布の particle として保持する。NRPA の添字を異なる
state-action 間で共有すると、それらは意図的に同じ logit を学習する。

## English

This header provides PUCT, per-node AMAF and RAVE statistics, proof-propagating
MCTS-Solver, visit-dependent Progressive Widening, stochastic Double Progressive
Widening, decaying Progressive Bias, public rollouts, genuinely nested NMCS, and
policy-adapting NRPA.

The game callbacks and result fields are specified in “共通のゲーム API と結果”.
PUCT normalizes and uses `prior(state, move)` in both expansion and
$Q+cP\sqrt N/(1+n)$ selection. RAVE blends direct and same-player AMAF estimates;
AMAF uses those all-moves-as-first estimates directly. MCTS-Solver propagates
existential wins/losses immediately and propagates draw or the opposite result
after every child is expanded and proven.

Progressive Widening admits at most
$\max(1,\lfloor kN^\alpha\rfloor)$ children. DPW applies this bound independently
to actions and sampled stochastic outcomes. Progressive Bias adds
`weight * heuristic / (1 + visits)`. The rollout policy returns an index into
the supplied move vector.

NMCS performs a level-lower nested search for every choice at every visited
state. NRPA performs $R^\ell$ level-0 rollouts, keeps a best sequence, and applies
the softmax log-policy gradient after every lower-level call; `nrpa` is an alias
of `nested_rollout_policy_adaptation`.

The detailed per-operation bounds are listed in “計算量”. Moves must be copyable;
AMAF/RAVE and NRPA also require equality. Scores, priors, heuristic values, and
parameters are validated as described in “注意点”. A depth-limited evaluator must
handle nonterminal states. Fixed engine state and fixed legal-move order make
the algorithms reproducible.
