---
title: Monte Carlo Tree Search
documentation_of: ../src/approximate/search/monte_carlo_tree_search.hpp
---

UCT による二人零和ゲーム向け Monte Carlo Tree Search。

# API

```cpp
monte_carlo_tree_search(root, iterations,
    terminal, legal_moves, transition, player_to_move, reward,
    random_engine, exploration = sqrt(2),
    maximum_rollout_depth = 256)
```

- `terminal(state)`: 終局なら真。
- `legal_moves(state)`: 合法手の range。
- `transition(state, move)`: 遷移後の新しい状態。
- `player_to_move(state)`: 手番識別子。等値比較できればよい。
- `reward(state, root_player)`: 根プレイヤー視点の報酬。通常は $[-1,1]$。
- `random_engine`: `std::mt19937` などの UniformRandomBitGenerator。

木内の選択では根手番の節点が平均報酬を最大化し、相手手番の節点が最小化する。ロールアウトは合法手から一様ランダムに選ぶ。反復終了後、根の子のうち訪問数最大の手を返す。

```cpp
template<class Move>
struct MonteCarloTreeSearchResult {
    optional<Move> move;
    size_t visits;
    double average_reward;
    size_t tree_nodes;
};
```

合法手がない、または展開される前に反復が終わった場合は `move == nullopt`。`visits` と `average_reward` は選択手の統計、`tree_nodes` は根を含む構築ノード数。

# 計算量

反復回数を $I$、木選択の最大深さを $D$、ロールアウト上限を $L$、合法手生成と遷移の1回のコストをそれぞれ $G,T$ とする。子選択で全子を走査する実装なので、展開済み分岐数の最大を $b$ とすると時間計算量は $O(I(D(b+G)+L(G+T)))$。

木ノード数を $N$、各ノードに保存する未展開手の総数を $M$ とすると空間計算量は $O(N+M)$。ロールアウト作業領域は、その時点の合法手数に比例する。

# 前提・例外

`exploration` は有限かつ非負でなければならず、違反時は `std::invalid_argument`。`reward` は最大ロールアウト深さで非終局の状態も評価できなければならず、有限値を返す必要がある。同じ乱数エンジン状態・同じ合法手順序なら結果は再現可能。
