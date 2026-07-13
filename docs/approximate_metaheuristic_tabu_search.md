---
title: Tabu Search (タブー探索)
documentation_of: ../src/approximate/metaheuristic/tabu_search.hpp
---

## `tabu_search`

```cpp
auto result = approximate::metaheuristic::tabu_search(
    initial, neighborhood, evaluate, key,
    iterations, tabu_tenure, hash, compare
);
```

各反復で`neighborhood(current)`内の許可された最良候補へ移る。
直前に離れた状態の`key(state)`を`tabu_tenure`反復だけ禁止し、
全体最良を更新する候補にはaspiration条件を適用して禁止を解除する。
既定では目的値を最小化する。`key`の戻り値は等値比較でき、`hash`でhash可能で
なければならない。tenureが0なら`std::invalid_argument`を送出する。

## 計算量

$I$を反復数、反復$i$の近傍数を$D_i$、評価を$E$とする。

| API・操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `tabu_search` | 期待$O(E\sum_iD_i)$ | $O(I+\max_iD_i)$ |
| `SearchResult`の各member参照 | $O(1)$ | $O(1)$ |

hash表が衝突攻撃を受ける場合の最悪時間計算量は $O(I\sum_iD_i)$である。

## 注意点

目的関数・近傍生成器が返す値とrangeは、その呼び出し中に有効でなければならない。探索はheuristicであり、明記した停止条件を除いて大域最適性や収束を保証しない。
