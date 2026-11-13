---
title: Mo's Algorithm with Modifications (変更付きMo法)
documentation_of: ../../../../src/algorithm/range/offline_query/mo_with_modifications.hpp
---

点の変更をはさむ区間クエリを、左端・右端・時刻の 3 軸で並べ替えて処理する（ブロック幅 $N^{2/3}$）。

## API

- `MoWithModifications(n)`: 列長を設定する。`n < 0` なら `invalid_argument` を送出する。
- `add_query(l, r, time)`: 変更を `time` 個適用した時点での半開区間 $[l, r)$ のクエリを追加し、0 から始まるクエリ番号を返す。$0\le l\le r\le n$ かつ `time >= 0` でなければならず、違反時は `out_of_range` を送出する。
- `query_count()`: クエリ数を返す。
- `run(add_left, add_right, remove_left, remove_right, apply_update, rollback_update, answer) const`: 時刻を先に合わせてから端点を動かし、各コールバックを呼ぶ。`apply_update(t, left, right)` は $t$ 番目（0-indexed）の変更を適用し、`rollback_update(t, left, right)` はそれを取り消す。クエリがなければ何もしない。

## 時間計算量

- `add_query`: 償却 $O(1)$、`query_count`: $O(1)$。
- `run`: 並べ替え $O(Q\log Q)$、端点・時刻の移動は典型的に $O((N+Q)N^{2/3})$ 回。

## 注意点

- 変更のコールバックには現在の区間 `left`, `right`（半開区間）も渡されるため、変更位置が区間内なら寄与を更新できる。
- 変更の総数は呼び出し側が管理する。`time` が実際の変更数を超えないようにすること。
