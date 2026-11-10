---
title: Rollback Mo's Algorithm (ロールバックMo法)
documentation_of: ../../../../src/algorithm/range/offline_query/rollback_mo.hpp
---

要素の追加と状態の巻き戻しだけで区間クエリを処理する Mo 法である。
削除操作を実装しにくい集約（最大値、連結成分数など）に向く。

## API

- `RollbackMo(n)`: 列長 $n$ を設定する。$n<0$ のとき `std::invalid_argument` を送出する。
- `add_query(l, r)`: 半開区間 `[l, r)` のクエリを追加し、0 から順に振られる ID を返す。`0 <= l <= r <= n` を満たさないとき `std::out_of_range` を送出する。空区間も可。
- `run(add, snapshot, rollback, answer)`: 全クエリを処理する。各コールバックは次のとおり。
  - `add(p)`: 位置 `p` の要素を現在の状態に追加する。
  - `snapshot()`: 現在の状態を表すトークンを返す。
  - `rollback(token)`: `snapshot()` が返したトークンの時点まで状態を戻す。
  - `answer(id)`: 現在の状態がクエリ `id` の区間に一致しているときに呼ばれる。

## 時間計算量

ブロック幅を $B=\lfloor\sqrt n\rfloor$ とし、クエリ数を $Q$ とする。

- `add_query`: 償却 $O(1)$。
- `run`: `add` の呼び出しは $O((n+Q)\sqrt n)$ 回、`snapshot` と `rollback` はそれぞれ $O(Q+n/B)$ 回である。コールバックがすべて $O(1)$ なら全体で $O((n+Q)\sqrt n+Q\log Q)$。

## 注意点

- `snapshot()` の戻り値は `rollback(token)` へそのまま渡せる必要がある。トークンは入れ子に使われる（ブロック単位のスナップショットの内側でクエリ単位のスナップショットを取る）。
- `run` は `const` であり、何度でも呼べる。`answer` はクエリの追加順ではなく、ソート後の順で呼ばれる。
