---
title: Advanced Adversarial and Stochastic Search (高度な敵対・確率ゲーム木探索)
documentation_of: ../../../src/approximate/search/advanced_adversarial_search.hpp
---

`advanced_adversarial_search` 名前空間に、深さ制限付きゲーム木の反復探索、選択的枝刈り、手順順序、確率・多人数探索、モンテカルロ探索をまとめる。既存の `adversarial_search.hpp` と同時に include できる。

`children(state)` は次状態の範囲を返し、`evaluate(state)` は二人用 API では根のプレイヤーから見た値を返す。同値なら入力の範囲で先にあるものを選ぶ。すべての結果は `score`、根を含む `principal_variation`、`completed_depth`、次の統計を返す。

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

分岐数を $b$、通常探索深さを $d$、静止探索深さを $q$、反復回数を $I$ とする。主変化（PV）の構築を含め、再帰中の追加空間計算量は最悪 $O(d^2)$ である。

## 時間計算量

各操作の時間計算量は以下の各 API の節に個別に記す。枝刈り系で保証される最悪時間計算量は、明記した probe の係数を除けば、完全なゲーム木の探索と同じ $O(b^d)$ である。

## 厳密探索ドライバ

```cpp
mtdf_search(root, depth, first_guess, evaluate, children,
            maximizing_player = true)
aspiration_search(root, depth, guess, initial_window, evaluate, children,
                  maximizing_player = true)
iterative_deepening_search(root, maximum_depth, evaluate, children,
                           maximizing_player = true)
```

- `mtdf_search` は整数の評価値に対して zero-window alpha-beta を上下界が一致するまで反復する。呼び出し回数を $p$ として時間計算量は $O(p b^d)$、空間は $O(d^2)$。
- `aspiration_search` は窓 `[guess - initial_window, guess + initial_window]` から始め、fail-low / fail-high のたびに窓の幅を倍にして再探索する。再探索回数を $p$ として時間計算量は $O(p b^d)$、空間は $O(d^2)$。
- `iterative_deepening_search` は深さ 0 から順に探索し、直前の PV を優先する。時間計算量は $\sum_{i=0}^d O(b^i)=O(b^d)$、空間は $O(d^2)$。

## 選択的探索

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

- Quiescence Search は stand-pat（静的評価で打ち切る選択肢）と `noisy_children` だけを最大 $q$ 手探索する。時間は $O(b_q^q)$。
- Null-Move Pruning は `null_transition(state)` で手番を渡し、浅い探索での fail-high / fail-low を通常の枝のカットに使う。最悪時間は $O(b^d)$。
- Late Move Reduction (LMR) は良い手の順序を仮定して後半の手を浅く探索し、窓の中で改善した手だけを全深さで再探索する。最悪時間は $O(b^d)$。
- Futility Pruning は浅い節点の静的評価値が `margin` を加減しても境界へ届かないとき枝を省く。最悪時間は $O(b^d)$。
- Razoring は浅い節点が境界より十分悪いとき Quiescence Search で確認して通常の探索を省く。最悪時間は $O(b^d+b_q^q)$。
- ProbCut は `margin` を越える浅い null-window probe を統計的なカット根拠にする。Multi-ProbCut は複数の `(margin, reduction, minimum_depth)` probe を順に試す。probe 数を $k$ として最悪時間はそれぞれ $O(b^d)$、$O(kb^d)$。

`NullMoveOptions{reduction, minimum_depth}`、`LateMoveReductionOptions{full_depth_moves, reduction, minimum_depth}`、各枝刈りのオプションは、正の深さと非負の `margin` を要求する。

## 手の順序付け

```cpp
KillerHeuristic<Key> killers;
HistoryHeuristic<Key> history;
CountermoveHeuristic<Key> countermoves;

killer_heuristic_search(root, depth, evaluate, children, move_key, killers)
history_heuristic_search(root, depth, evaluate, children, move_key, history)
countermove_heuristic_search(root, depth, evaluate, children, move_key, countermoves)
```

`move_key(parent, child)` はハッシュ可能な手の識別子を返す。Killer は ply ごとに直近 2 個のカットを起こした手を、History はカットのたびに $\mathrm{depth}^2$ を加算した値が大きい手を、Countermove は直前の手ごとにカットを起こした応手を、それぞれ先頭へ移す。表は次の反復へ再利用でき、`clear()` で消去できる。各探索の最悪時間計算量はソートを含めて $O(b^d\log b)$、探索の空間計算量は $O(d^2)$、表の空間計算量は記録したキーの数を $M$ として $O(M+d)$。

## ビーム・確率節点

```cpp
beam_minimax_search(root, depth, beam_width, evaluate, children)

expectimax_search(root, depth, evaluate, children, node_type, probability)
```

Beam Minimax は、各 ply で静的評価が上位 `beam_width` 個の状態だけを再帰的に探索する。幅を $w$ として、時間計算量は $O(b\log w + w^d)$、空間計算量は $O(d^2+bd)$。`node_type(state)` は `ExpectimaxNode::{maximum, minimum, chance}` を返す。確率節点の `probability(parent, child)` は有限かつ非負でなければならず、実装は合計で正規化する。Expectimax の時間計算量は $O(b^d)$、空間計算量は $O(d^2)$。確率節点の PV には、確率最大の枝を代表として載せる。

## 多人数探索

```cpp
maxn_search(root, depth, player_count, evaluate_vector, children,
            player_to_move)
paranoid_search(root, depth, root_player, evaluate_for_root, children,
                player_to_move)
```

MaxN は手番のプレイヤーが自身の成分を最大化し、`MultiPlayerSearchResult` に全プレイヤーの値を返す。評価値の範囲の長さは常に `player_count` と一致させる。Paranoid Search は、根のプレイヤーを最大化側、他の全員を共同の最小化側として alpha-beta 探索する。人数を $p$ として、どちらも最悪時間計算量 $O(b^d)$、空間計算量 $O(d^2+pd)$。

## モンテカルロ探索

```cpp
flat_monte_carlo_search(root, simulations, maximum_rollout_depth,
                        evaluate, children, random_engine,
                        maximizing_player = true)
monte_carlo_search(root, iterations, maximum_tree_depth,
                   evaluate, children, random_engine,
                   exploration = sqrt(2), maximizing_player = true)
```

Flat Monte Carlo Search は、根の各子へ順番に（round-robin で）一様ランダムなロールアウトを割り当て、平均評価が最良の子を返す。Monte Carlo Search は節点を保持し、UCT による選択・ 1 手の展開・ランダムなロールアウト・逆伝播を繰り返す。ロールアウトの深さを $r$ とすると、時間計算量は Flat が $O(b+Irb)$、UCT が素朴な子の走査を含めて $O(Irb)$、空間計算量はそれぞれ $O(b+br)$ と $O(Ib)$。`simulations == 0` / `iterations == 0` の場合や合法手がない場合は、根を評価して返す。

## 注意点

- 選択的枝刈り、Beam、Monte Carlo は近似手法であり、誤った静的評価、悪い手の順序、ツークツワンク、確率モデルの不一致によって Minimax 値を失うことがある。厳密な値が必要なら、MTD(f)、Aspiration、Iterative Deepening、または十分に安全側のオプションを使う。
- 不正なパラメータ（正でない窓・幅・深さ、負の `margin`、空の probe 列、不正な確率、2 人未満の `player_count`、長さの合わない評価値、負または有限でない `exploration` など）に対しては `std::invalid_argument` を、MaxN で手番のプレイヤー番号が範囲外なら `std::out_of_range` を送出する。
- Null Move の PV には `null_transition` の状態が現れることがある。

- 整数の API では、評価値と `margin` の加減算の結果が `numeric_limits<Score>::lowest() / 4` から `max() / 4` の範囲に収まるようにする。
- 状態は子の範囲からコピーまたはムーブできなければならない。反復深化で PV の順序を使う状態は `operator==` を持ち、手順序付けのキーは安定した `hash` と等値比較を持たなければならない。
- 同じ乱数のシードでも、`children` の順序を変えると Monte Carlo の結果は変わる。
