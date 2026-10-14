---
title: Offline Range Mode Queries (区間最頻値のオフライン一括照会) [static_range_mode_query]
documentation_of: ../../../../src/algorithm/range/offline_query/range_mode_queries.hpp
---

`range_mode_queries(values, ranges)` は各非空半開区間の `(最頻値, 出現数)` を入力順に返す。
値型は比較・等値比較・コピーが可能であること。負数や重複を許す。同頻度の最頻値は任意の一つを返す。
Mo の区間走査と頻度別の双方向リストを使う。

## 時間計算量

配列長 $N$、照会数 $Q$ とすると、$Q>0$ で $O(N\log N+Q\log Q+N\sqrt Q+Q)$ 時間、$O(N+Q)$ 空間。
`Q=0` では $O(N\log N)$ 時間。

## 注意点

各範囲は `0 <= l < r <= N`。違反時は `out_of_range`。
全照会を事前に受け取る。返す最頻値の同率時の選択順序に依存しないこと。
