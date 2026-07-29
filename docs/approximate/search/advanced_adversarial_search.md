---
title: Advanced Adversarial and Stochastic Search (高度な敵対・確率ゲーム木探索)
documentation_of: ../../../src/approximate/search/advanced_adversarial_search.hpp
---

`advanced_adversarial_search` 名前空間に、深さ制限ゲーム木の反復探索、選択的枝刈り、手順順序、確率・多人数探索、モンテカルロ探索をまとめる。既存の `adversarial_search.hpp` と同時に include できる。

`children(state)` は次状態の range、`evaluate(state)` は二人用 API では根プレイヤー基準の値を返す。同値なら入力 range の先頭を選ぶ。すべての結果は `score`、根を含む `principal_variation`、`completed_depth`、次の統計を返す。

```cpp
struct SearchStatistics {
    size_t visited_nodes, evaluated_nodes, cutoffs, re_searches;
    size_t reduced_searches, heuristic_prunes, generated_nodes, simulations;
};

template<class State, class Score>
struct SearchResult {
    Score score;
    vector<State> principal_variation;
    SearchStatistics statistics;
    size_t completed_depth;
};
```

分岐数を $b$、通常探索深さを $d$、静止探索深さを $q$、反復回数を $I$ とする。PV の構築を含む再帰中の追加空間は最悪 $O(d^2)$ である。

## 時間計算量

各操作の時間計算量は以下の各 API 節に個別に記す。枝刈り系の保証される最悪計算量は、明記した probe 係数を除けば完全なゲーム木と同じ O(b^d) である。

## Exact search drivers (厳密探索ドライバ)

```cpp
mtdf_search(root, depth, first_guess, evaluate, children,
            maximizing_player = true)
aspiration_search(root, depth, guess, initial_window, evaluate, children,
                  maximizing_player = true)
iterative_deepening_search(root, maximum_depth, evaluate, children,
                           maximizing_player = true)
```

- `mtdf_search` は整数評価値に対し zero-window alpha-beta を上下界が一致するまで反復する。呼出し回数を $p$ として時間は $O(p b^d)$、空間は $O(d^2)$。
- `aspiration_search` は `guess ± initial_window` から開始し、fail-low/high のたび窓を倍増して再探索する。再探索回数を $p$ として時間は $O(p b^d)$、空間は $O(d^2)$。
- `iterative_deepening_search` は深さ 0 から順に探索し、直前の PV を優先する。時間は $\sum_{i=0}^d O(b^i)=O(b^d)$、空間は $O(d^2)$。

## Selective search (選択的探索)

```cpp
quiescence_search(root, maximum_noisy_depth, evaluate, noisy_children,
                  maximizing_player = true)
null_move_pruning_search(root, depth, evaluate, children, null_transition,
                         NullMoveOptions, maximizing_player = true)
late_move_reduction_search(root, depth, evaluate, children,
                           LateMoveReductionOptions, maximizing_player = true)
futility_pruning_search(root, depth, evaluate, children,
                        FutilityOptions<Score>, maximizing_player = true)
razoring_search(root, depth, evaluate, children, noisy_children,
                RazoringOptions<Score>, maximizing_player = true)
probcut_search(root, depth, evaluate, children, ProbCutOptions<Score>,
               maximizing_player = true)
multi_probcut_search(root, depth, evaluate, children,
                     MultiProbCutOptions<Score>, maximizing_player = true)
```

- Quiescence Search は stand-pat と `noisy_children` だけを最大 $q$ 手探索する。時間は $O(b_q^q)$。
- Null-Move Pruning は `null_transition(state)` で手番を渡し、浅い fail-high/fail-low を通常枝のカットに使う。最悪時間は $O(b^d)$。
- Late Move Reduction (LMR) は良い順序を仮定して後半手を短縮探索し、窓内で改善した手だけ全深さで再探索する。最悪時間は $O(b^d)$。
- Futility Pruning は浅い節点の静的値が `margin` を加減しても境界へ届かないとき枝を省く。最悪時間は $O(b^d)$。
- Razoring は浅い節点が境界より十分悪いとき Quiescence Search で確認して通常探索を省く。最悪時間は $O(b^d+b_q^q)$。
- ProbCut は `margin` を越える浅い null-window probe を統計的なカット根拠にする。Multi-ProbCut は複数の `(margin, reduction, minimum_depth)` probe を順に試す。probe 数を $k$ として最悪時間はそれぞれ $O(b^d)$、$O(kb^d)$。

`NullMoveOptions{reduction, minimum_depth}`、`LateMoveReductionOptions{full_depth_moves, reduction, minimum_depth}`、各枝刈り option は正の深さと非負 margin を要求する。

## Move ordering (手順順序)

```cpp
KillerHeuristic<Key> killers;
HistoryHeuristic<Key> history;
CountermoveHeuristic<Key> countermoves;

killer_heuristic_search(root, depth, evaluate, children, move_key, killers)
history_heuristic_search(root, depth, evaluate, children, move_key, history)
countermove_heuristic_search(root, depth, evaluate, children, move_key, countermoves)
```

`move_key(parent, child)` はハッシュ可能な手識別子を返す。Killer は ply ごとに直近 2 個の cutoff 手、History は cutoff に `depth²` を加算した手、Countermove は直前手ごとの cutoff 応手を先頭へ移す。テーブルを次の反復へ再利用でき、`clear()` で消去できる。各探索の最悪時間は sort を含め $O(b^d\log b)$、探索空間は $O(d^2)$、表は記録キー数を $M$ として $O(M+d)$。

## Beam and stochastic nodes (ビーム・確率節点)

```cpp
beam_minimax_search(root, depth, beam_width, evaluate, children)

expectimax_search(root, depth, evaluate, children, node_type, probability)
```

Beam Minimax は各 ply で静的評価上位 `beam_width` 状態だけを再帰探索する。時間は $O(b\log w + w^d)$、空間は $O(d^2+bd)$（$w$ は幅）。`node_type(state)` は `ExpectimaxNode::{maximum, minimum, chance}` を返す。Chance 節点の `probability(parent, child)` は有限・非負で、実装は合計で正規化する。Expectimax の時間は $O(b^d)$、空間は $O(d^2)$。Chance 節点の PV は最大確率の枝を代表として載せる。

## Multiplayer search (多人数探索)

```cpp
maxn_search(root, depth, player_count, evaluate_vector, children,
            player_to_move)
paranoid_search(root, depth, root_player, evaluate_for_root, children,
                player_to_move)
```

MaxN は手番プレイヤーが自身の成分を最大化し、`MultiPlayerSearchResult` に全プレイヤーの値を返す。評価 range の長さは常に `player_count` と一致させる。Paranoid Search は根プレイヤーを最大側、他の全員を共同の最小側として alpha-beta 探索する。どちらも最悪時間 $O(b^d)$、空間 $O(d^2+pd)$（$p$ は人数）。

## Monte Carlo searches (モンテカルロ探索)

```cpp
flat_monte_carlo_search(root, simulations, maximum_rollout_depth,
                        evaluate, children, random_engine,
                        maximizing_player = true)
monte_carlo_search(root, iterations, maximum_tree_depth,
                   evaluate, children, random_engine,
                   exploration = sqrt(2), maximizing_player = true)
```

Flat Monte Carlo Search は根の各子へ round-robin で一様ランダム rollout を割り当て、平均評価が最良の子を返す。Monte Carlo Search は節点を保持し、UCT の selection・一手 expansion・random rollout・backpropagation を反復する。深さを $r$ とすると時間は Flat が $O(I(br+r b))$ のうち初回列挙をまとめて $O(b+Irb)$、UCT が素朴な子走査を含め $O(I(br+r b))$、空間はそれぞれ $O(b+br)$ と $O(Ib)$。`simulations == 0`/`iterations == 0` または合法手なしでは根を評価して返す。

## 注意点

選択的枝刈り、Beam、Monte Carlo は近似手法であり、誤った静的評価、悪い手順順序、zugzwang、確率モデル不一致により Minimax 値を失い得る。厳密値が必要なら MTD(f)、Aspiration、Iterative Deepening または十分安全側の option を使う。Null Move の PV には `null_transition` 状態が現れる場合がある。

整数 API では評価値と `margin` の加減が `numeric_limits<Score>::lowest()/4` と `max()/4` の範囲内に収まるようにする。状態は子 range からコピーまたは move 可能であること、反復深化で PV 順序を使う状態は `operator==` を持つこと、heuristic key は安定した `hash` と等値比較を持つことが必要である。同じ乱数 seed でも `children` の順序変更で Monte Carlo の結果は変わる。
