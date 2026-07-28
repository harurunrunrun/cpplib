---
title: Mo's Algorithm with Modifications (変更付きMo法)
documentation_of: ../../../../src/algorithm/range/offline_query/mo_with_modifications.hpp
---

点変更を挟む静的な区間クエリを、左右端と時刻の3軸で並べ替えて処理する。

## API

- `MoWithModifications(n)`: 列長を設定する。
- `add_query(l,r,time)`: 時刻 `time` の半開区間を追加しIDを返す。
- `query_count()`: クエリ数を返す。
- `run(...)`: 端点移動、変更適用・取消、回答保存の各コールバックを実行する。

## 時間計算量

- `add_query`, `query_count`: 償却 $O(1)$。
- `run`: 並べ替え $O(Q\log Q)$、端点・時刻移動は典型的に $O((N+Q)N^{2/3})$ 回。

## 注意点

- 更新コールバックには現在の区間 `[left,right)` も渡される。
