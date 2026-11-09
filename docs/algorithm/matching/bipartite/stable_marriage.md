---
title: Stable Marriage (安定結婚)
documentation_of: ../../../../src/algorithm/matching/bipartite/stable_marriage.hpp
---

二部集合の両側がそれぞれ相手への厳密な選好順序を持つとき、ブロッキングペアのない安定マッチングを求める。

## API

```cpp
StableMarriageResult stable_marriage(
    const vector<vector<int>>& proposer_preferences,
    const vector<vector<int>>& acceptor_preferences
)
```

提案側を固定した Gale–Shapley 法を実行する。各選好列は相手の一部だけを含んでもよい。片側だけが相手を許容している組はマッチしない。

```cpp
struct StableMarriageResult {
    vector<int> proposer_match;
    vector<int> acceptor_match;
};
```

- `proposer_match[p]`: 提案者 `p` の相手。未マッチなら `-1`
- `acceptor_match[a]`: 受諾者 `a` の相手。未マッチなら `-1`

## 時間計算量

提案者数を $P$、受諾者数を $A$、両側の選好列の長さの総和を $L$ とする。

- 選好の検査と受諾者の順位表の構築: $O(PA + L)$
- Gale–Shapley 法: $O(P + L)$
- 全体: $O(PA + L)$

## 空間計算量

$O(PA + L)$。

## 注意点

選好列の相手番号は対応する側の範囲内にあり、同じ列の中で重複してはならない。違反時は `runtime_error` を送出する。同順位は扱わない。戻り値は提案側にとって最適な安定マッチングである。安定マッチングは一意とは限らない。
