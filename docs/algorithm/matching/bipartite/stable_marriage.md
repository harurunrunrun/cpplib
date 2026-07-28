---
title: Stable Marriage (安定結婚)
documentation_of: ../../../../src/algorithm/matching/bipartite/stable_marriage.hpp
---

二部集合の両側が相手への厳密な選好順序を持つとき、ブロッキングペアのない安定マッチングを求める。

## API

```cpp
StableMarriageResult stable_marriage(
    const vector<vector<int>>& proposer_preferences,
    const vector<vector<int>>& acceptor_preferences
)
```

提案側を固定したGale--Shapley法を実行する。各選好列は一部の相手だけを含んでもよく、片側だけが相手を許容している組はマッチしない。

```cpp
struct StableMarriageResult {
    vector<int> proposer_match;
    vector<int> acceptor_match;
};
```

- `proposer_match[p]`: 提案者 `p` の相手。未マッチなら `-1`
- `acceptor_match[a]`: 受諾者 `a` の相手。未マッチなら `-1`

## 時間計算量

提案者数を $P$、受諾者数を $A$、両側の選好列長の総和を $L$ とする。

- 選好の検査と受諾者順位表の構築: $O(PA+L)$
- Gale--Shapley法: $O(L)$
- 全体: $O(PA+L)$

## 空間計算量

$O(PA+L)$。

## 注意点

選好列の相手番号は対応する側の範囲内で、同じ列に重複してはならない。違反時は `runtime_error` を送出する。同順位は扱わない。戻り値は提案側にとって最適な安定マッチングであり、安定マッチングが一意とは限らない。
