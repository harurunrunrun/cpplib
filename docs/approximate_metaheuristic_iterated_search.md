---
title: Iterated Metaheuristic Search (反復型メタヒューリスティック探索)
documentation_of: ../src/approximate/metaheuristic/iterated_search.hpp
---

`approximate::metaheuristic`名前空間で、目的値を既定で最小化する反復型探索を提供する。
`local_improve(state)`は局所改善後の状態、`evaluate(state)`は目的値を返す。
`Compare`を変更すれば最大化にも利用できる。

## `iterated_local_search`

```cpp
auto result = iterated_local_search(
    initial, local_improve, perturb, evaluate, iterations, rng, compare
);
```

`perturb(current, rng)`で摂動し、局所改善後に現在解より良ければ採用する。

## `variable_neighborhood_descent`

```cpp
auto result = variable_neighborhood_descent(
    initial, neighborhoods, evaluate, maximum_moves, compare
);
```

同じcallable型のrange、または`std::function`のrangeとして近傍群を渡す。
現在の近傍で最良改善を行えたら先頭近傍へ戻り、できなければ次へ進む。

## `variable_neighborhood_search`

```cpp
auto result = variable_neighborhood_search(
    initial, shakings, local_improve, evaluate, iterations, rng, compare
);
```

`shakings[k](current, rng)`で近傍$k$へ移して局所改善する。改善時は近傍0へ、
非改善時は次の近傍へ進む。

## `grasp`

```cpp
auto result = grasp(
    construct, local_improve, evaluate, iterations, rng, compare
);
```

`construct(rng)`によるランダム化構築と局所改善を繰り返す。
反復数0では`std::invalid_argument`を送出する。

## `destroy_and_repair`

```cpp
auto result = destroy_and_repair(
    initial, destroy, repair, evaluate, iterations, rng, compare
);
```

`destroy(current, rng)`の部分解を`repair(partial, rng)`で修復し、改善時に採用する。
Large Neighborhood Search、Ruin and Recreateの基本部品として使える。

## 計算量

$I$を反復数、局所改善を$L$、摂動・shakingを$P$、評価を$E$、
近傍$i$の列挙数を$D_i$とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `iterated_local_search` | $O(L+E+I(P+L+E))$ | $O(1)$ |
| `variable_neighborhood_descent` | $O(E\sum_iD_i)$（実際に調べた近傍分） | $O(\max_iD_i)$ |
| `variable_neighborhood_search` | $O(L+E+I(P+L+E))$ | $O(1)$ |
| `grasp` | $O(I(G+L+E))$ | $O(1)$ |
| `destroy_and_repair` | $O(E+I(D+R+E))$ | 部分解1個分 |

## 注意点

目的関数・近傍生成器が返す値とrangeは、その呼び出し中に有効でなければならない。探索はheuristicであり、明記した停止条件を除いて大域最適性や収束を保証しない。
