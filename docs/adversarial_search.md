---
title: Adversarial Game-Tree Search (敵対ゲーム木探索)
documentation_of: ../src/approximate/search/adversarial_search.hpp
---

二人零和・完全情報ゲーム向けの深さ制限ゲーム木探索。`children(state)` は次状態の range、`evaluate(state)` は評価値を返す。子がない状態は深さが残っていても葉として評価する。

```cpp
template<class State, class Score>
struct GameTreeSearchResult {
    Score score;
    vector<State> principal_variation;
    size_t visited_nodes;
};
```

`principal_variation` は根から選択した葉までを含む。評価値が同じ子では `children` の先頭を選ぶ。

## `minimax_search`

```cpp
minimax_search(root, depth, evaluate, children,
               maximizing_player = true)
```

最大手番と最小手番を交互に適用する。`evaluate` は常に同じ基準プレイヤーから見た値を返す。

分岐数を $b$、深さを $d$ とすると時間計算量は $O(b^d)$、再帰スタックは $O(d)$。主変化列の構築を含む追加空間計算量の最大は $O(d^2)$。

## `alpha_beta_search`

```cpp
alpha_beta_search(root, depth, evaluate, children,
                  maximizing_player = true)
```

Minimax と同じ値を alpha-beta 枝刈りで求める。最良順序の時間計算量は $O(b^{d/2})$、最悪は $O(b^d)$。追加空間計算量は $O(d^2)$。

## `negamax_search`

```cpp
negamax_search(root, depth, evaluate, children)
```

`max(a,b)=-min(-a,-b)` を利用した Minimax の対称形。`evaluate(state)` は、その状態で手番を持つプレイヤーから見た値を返す。時間計算量は $O(b^d)$、追加空間計算量は $O(d^2)$。
評価値型は符号付き数値型でなければならない。

## `principal_variation_search` / `negascout_search`

```cpp
principal_variation_search(root, depth, evaluate, children)
negascout_search(root, depth, evaluate, children)
```

最初の子を通常窓、残りを null-window で調べ、必要なときだけ再探索する NegaScout/PVS。両関数は同じ探索を行う。評価値型は符号付き整数で、Negamax と同じく手番プレイヤー視点の評価が必要。

良い手順序では alpha-beta より再探索を減らせる。最悪時間計算量は $O(b^d)$、追加空間計算量は $O(d^2)$。

## 注意点

評価値の比較と Negamax の符号反転が正しく定義される必要がある。PVS/NegaScout の全評価値の絶対値は `numeric_limits<Score>::max()/4` 未満でなければならない。
