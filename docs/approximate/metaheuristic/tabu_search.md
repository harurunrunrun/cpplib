---
title: Tabu Search (タブー探索)
documentation_of: ../../../src/approximate/metaheuristic/tabu_search.hpp
---

## `tabu_search`

```cpp
auto result = approximate::metaheuristic::tabu_search(
    initial, neighborhood, evaluate, key,
    iterations, tabu_tenure, hash = {}, compare = {}
);
```

各反復で `neighborhood(current)` のうち許可された最良の候補へ移る。直前に離れた状態の `key(state)` を `tabu_tenure` 反復の間禁止し、全体の最良解を更新する候補には特例（aspiration）条件を適用して禁止を解除する。デフォルトでは目的値を最小化する。`key` の返り値は等値比較でき、`hash` でハッシュ可能でなければならない。

## 計算量

$I$ を反復回数、反復 $i$ の近傍数を $D_i$、評価 1 回のコストを $E$ とする。

| API・操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `tabu_search` | 期待 $O(E\sum_iD_i)$ | $O(I+\max_iD_i)$ |
| `SearchResult` の各メンバの参照 | $O(1)$ | $O(1)$ |

ハッシュ表が衝突を狙った入力を受ける場合、最悪の時間計算量は $O(I\sum_iD_i)$ である。

## 注意点

- `tabu_tenure == 0` なら `std::invalid_argument` を送出する
- 目的関数・近傍生成器が返す値と range は、その呼び出しの間有効でなければならない
- 探索はヒューリスティックであり、明記した停止条件を除いて大域最適性や収束は保証しない
