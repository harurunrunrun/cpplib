---
title: Iterated Metaheuristic Search (反復型メタヒューリスティック探索)
documentation_of: ../../../src/approximate/metaheuristic/iterated_search.hpp
---

`approximate::metaheuristic` 名前空間で、デフォルトでは目的値を最小化する反復型の探索を提供する。`local_improve(state)` は局所改善後の状態を、`evaluate(state)` は目的値を返す。`Compare` を変更すれば最大化にも使える。

## `iterated_local_search`

```cpp
auto result = iterated_local_search(
    initial, local_improve, perturb, evaluate, iterations, rng, compare = {}
);
```

`perturb(current, rng)` で摂動し、局所改善した結果が現在解より良ければ採用する。

## `variable_neighborhood_descent`

```cpp
auto result = variable_neighborhood_descent(
    initial, neighborhoods, evaluate, maximum_moves, compare = {}
);
```

近傍群は、同じ呼び出し可能型の range、または `std::function` の range として渡す。現在の近傍で最良改善の遷移ができたら最初の近傍に戻り、できなければ次の近傍へ進む。

## `variable_neighborhood_search`

```cpp
auto result = variable_neighborhood_search(
    initial, shakings, local_improve, evaluate, iterations, rng, compare = {}
);
```

`shakings[k](current, rng)` で近傍 $k$ の解へ移してから局所改善する。改善したら近傍 0 に戻り、改善しなければ次の近傍へ進む。

## `grasp`

```cpp
auto result = grasp(
    construct, local_improve, evaluate, iterations, rng, compare = {}
);
```

`construct(rng)` によるランダム化構築と局所改善を繰り返す。

## `destroy_and_repair`

```cpp
auto result = destroy_and_repair(
    initial, destroy, repair, evaluate, iterations, rng, compare = {}
);
```

`destroy(current, rng)` が返す部分解を `repair(partial, rng)` で修復し、改善したら採用する。大近傍探索（LNS）や Ruin and Recreate の基本部品として使える。

## 計算量

$I$ を反復回数、局所改善のコストを $L$、摂動・シェイキングのコストを $P$、構築のコストを $G$、破壊・修復のコストを $D, R$、評価のコストを $E$、近傍 $i$ の列挙数を $D_i$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `iterated_local_search` | $O(L+E+I(P+L+E))$ | $O(1)$ |
| `variable_neighborhood_descent` | $O(E\sum_iD_i)$（実際に調べた近傍の分） | $O(\max_iD_i)$ |
| `variable_neighborhood_search` | $O(L+E+I(P+L+E))$ | $O(1)$ |
| `grasp` | $O(I(G+L+E))$ | $O(1)$ |
| `destroy_and_repair` | $O(E+I(D+R+E))$ | 部分解 1 個分 |

## 注意点

- `grasp` は `iterations == 0` で `std::invalid_argument` を送出する
- 目的関数・近傍生成器が返す値と range は、その呼び出しの間有効でなければならない
- 探索はヒューリスティックであり、明記した停止条件を除いて大域最適性や収束は保証しない
