---
title: Auctions and Fair Allocation Heuristics (オークションと公平配分ヒューリスティック)
documentation_of: ../../../src/approximate/graph/allocation.hpp
---

Utilities for one-to-one assignment and additive indivisible-item allocation.
一対一割当てと、加法的評価を持つ不可分財配分のための API 群である。

## Auction algorithms / オークション法

`auction_algorithm(values, epsilon)` implements the forward auction. Rows are
bidders, columns are items, and the number of items must be at least the number
of bidders. `epsilon_scaling_auction_algorithm` repeats auction phases while
preserving prices from `initial_epsilon` down to `final_epsilon`.

`auction_algorithm` は forward auction を実行する。行が bidder、列が item であり、item 数は bidder 数以上とする。`epsilon_scaling_auction_algorithm` は価格を phase 間で保持し、epsilon を縮小する。

`AuctionResult<Value>` contains `bidder_to_item`, `item_to_bidder`,
`total_value`, and the total number of bids across all phases.

With \(A\) bidders, \(I\) items, and \(B\) bids:

- Time / 時間計算量: \(O(BI)\)
- Extra space / 追加空間計算量: \(O(A+I)\)

The epsilon auction returns an epsilon-complementary-slack assignment; exact
optimality is not claimed for arbitrary epsilon.
epsilon auction は epsilon 相補スラック条件を満たす割当てを返すが、任意の epsilon に対する厳密最適性は保証しない。

## `random_serial_dictatorship`

Randomly permutes agents; each agent takes its most valuable remaining item.
Each agent receives at most one item.
agent をランダム順に並べ、各 agent が残りで最も高評価の item を1個取る。

- Time / 時間計算量: \(O(AI)\)
- Extra space / 追加空間計算量: \(O(A+I)\)

## `round_robin_allocation`

Cycles through `agent_order` until every item has been greedily picked. Repeated
agents in the order are allowed.
`agent_order` を巡回し、全 item が選ばれるまで各手番で最高評価の残余 item を取る。順序中の agent 重複を許す。

- Time / 時間計算量: \(O(AI+I^2)\)
- Extra space / 追加空間計算量: \(O(A+I)\), besides returned bundles

## `envy_cycle_elimination`

Allocates items in input order to an unenvied agent and rotates bundles along a
detected envy cycle. For additive nonnegative valuations this is the standard
EF1 construction.
入力順に item を非羨望 agent へ渡し、羨望 cycle が生じれば bundle を回転する。非負加法的評価では標準的な EF1 構成である。

- Time / 時間計算量: \(O((I+R)A(I+A))\)
- Here \(R\) is the number of bundle-cycle rotations. / bundle cycle の回転回数を \(R\) とする。
- Extra space / 追加空間計算量: \(O(A^2+A+I)\), besides returned bundles

## 注意点

`AllocationResult::item_owner[item] == agent_count` denotes an unallocated item;
this can occur in random serial dictatorship when items outnumber agents.
`item_owner[item] == agent_count` は未配分を表し、item が agent より多い serial dictatorship で生じ得る。行長不一致、非有限な浮動小数評価、不正な順序・epsilon には例外を送出する。
