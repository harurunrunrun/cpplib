---
title: Rollback Mo's Algorithm (ロールバックMo法)
documentation_of: ../../../../src/algorithm/range/offline_query/rollback_mo.hpp
---

追加と状態巻き戻しだけで区間クエリを処理するMo法。

## API

- `RollbackMo(n)`: 列長を設定する。
- `add_query(l,r)`: クエリを追加しIDを返す。
- `run(add,snapshot,rollback,answer)`: 位置追加、スナップショット、巻き戻し、回答を呼び出す。

## 時間計算量

- `add_query`: 償却 $O(1)$。
- `run`: コールバックが $O(1)$ なら $O((N+Q)\sqrt N+Q\log Q)$。

## 注意点

- `snapshot()` の戻り値は `rollback(token)` へそのまま渡せる必要がある。
