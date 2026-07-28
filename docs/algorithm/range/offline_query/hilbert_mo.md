---
title: Hilbert-order Mo's Algorithm (ヒルベルト順序Mo法)
documentation_of: ../../../../src/algorithm/range/offline_query/hilbert_mo.hpp
---

区間端点をHilbert曲線順に並べ、実用上の端点移動量を抑えるMo法。

## API

- `hilbert_order(x,y,bits,rotation)`: Hilbert順序値を返す。
- `HilbertMo(n)`: 列長を設定する。
- `add_query(l,r)`: クエリを登録する。
- `run(...)`: 左右への追加・削除と回答コールバックを実行する。

## 時間計算量

- `hilbert_order`: $O(bits)$。
- `add_query`: $O(\log N)$。`run` の並べ替えは $O(Q\log Q)$。
- 端点移動回数は入力依存で、典型的には通常のMo法と同程度以下。

## 注意点

- 区間は半開区間 `[l,r)`。
