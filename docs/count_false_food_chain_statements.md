---
title: Count False Food Chain Statements (食物連鎖の偽情報数) [CHAIN]
documentation_of: ../src/algorithm/graph/count_false_food_chain_statements.hpp
---

三すくみの種族関係について、順番に与えられる情報のうち偽となる個数を返す。

# FoodChainStatement

```cpp
struct FoodChainStatement{
    int relation;
    int left;
    int right;
};
```

`relation == 1` は2匹が同種、`relation == 2` は `left` が `right` を捕食するという情報を表す。

# count_false_food_chain_statements

```cpp
int count_false_food_chain_statements(
    int animal_count,
    const vector<FoodChainStatement>& statements
);
```

情報を先頭から処理し、範囲外・自己捕食・それ以前に真とされた情報との矛盾により偽となる情報数を返す。偽の情報は以後の制約へ追加しない。

## API別の時間計算量・空間計算量

動物数を $N$、情報数を $K$、逆 Ackermann 関数を $\alpha$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_false_food_chain_statements` | $O((N+K)\alpha(N))$ | $O(N)$ |

## 注意点

- 動物番号は `0` 以上 `animal_count` 未満である。
- 範囲外の動物番号は例外ではなく偽情報として数える。
- `relation` が `1`, `2` 以外、または `animal_count` が負の場合は `runtime_error` を送出する。
