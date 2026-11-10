---
title: Auctions and Fair Allocation Heuristics (オークションと公平配分ヒューリスティック)
documentation_of: ../../../src/approximate/graph/allocation.hpp
---

一対一の割当と、加法的な評価を持つ分割できない財の配分のための API 群である。`values[a][i]` はエージェント（入札者）`a` の財 `i` に対する評価値である。以下、エージェント数を $A$、財の数を $I$ とする。

## オークション法

```cpp
AuctionResult<Value> auction_algorithm(values, epsilon);
AuctionResult<Value> epsilon_scaling_auction_algorithm(
    values, initial_epsilon, final_epsilon, scaling_factor);
```

`auction_algorithm` は forward auction を実行する。行が入札者、列が財であり、財の数は入札者数以上でなければならない。`epsilon_scaling_auction_algorithm` は、価格をフェーズ間で引き継ぎながら、$\varepsilon$ を `initial_epsilon` から `scaling_factor` で割りつつ `final_epsilon` まで縮小してオークションのフェーズを繰り返す。

`AuctionResult<Value>` は、入札者から財への割当 `bidder_to_item`、財から入札者への割当 `item_to_bidder`、評価値の合計 `total_value`、全フェーズを通じた入札の総数 `bids` を持つ。

入札の総数を $B$ とすると、

- 時間計算量: $O(BI)$
- 追加空間計算量: $O(A+I)$

$\varepsilon$ オークションは $\varepsilon$ 相補スラック条件を満たす割当を返すが、任意の $\varepsilon$ に対する厳密な最適性は保証しない。

## `random_serial_dictatorship(values, random)`

エージェントをランダムな順に並べ、各エージェントが残りの財のうち最も評価の高いものを 1 個取る。各エージェントが受け取る財は高々 1 個である。

- 時間計算量: $O(AI)$
- 追加空間計算量: $O(A+I)$

## `round_robin_allocation(values, agent_order)`

全ての財が選ばれるまで `agent_order` を巡回し、各手番のエージェントが残りの財のうち最も評価の高いものを取る。順序の中でエージェントが重複してもよい。

- 時間計算量: $O(AI+I^2)$
- 追加空間計算量: 戻り値の各束を除いて $O(A+I)$

## `envy_cycle_elimination(values)`

財を入力順に、誰からも羨望されていないエージェントへ渡し、羨望の閉路が生じたら閉路に沿って束を回転させる。非負の加法的評価に対しては、EF1 を満たす配分を得る標準的な構成法である。

束の回転の回数を $R$ とすると、

- 時間計算量: $O((I+R)A(I+A))$
- 追加空間計算量: 戻り値の各束を除いて $O(A^2+A+I)$

## 注意点

- `AllocationResult` は各エージェントの束 `bundles` と各財の所有者 `item_owner` を持つ。`item_owner[item] == agent_count` は未配分を表し、財がエージェントより多い場合の `random_serial_dictatorship` で生じうる。
- 行の長さの不一致、非有限の浮動小数点の評価値、空の `agent_order` などの不正な順序、正でない $\varepsilon$、`initial_epsilon < final_epsilon`、1 以下の `scaling_factor`、財が入札者より少ない場合は `std::invalid_argument` を送出する。価格や合計が有限の範囲を超えた場合は `std::overflow_error` を送出する。
