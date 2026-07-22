---
title: Inconsistent Modulo-Three Constraints Count (法3差分制約の矛盾数) [CHAIN]
documentation_of: ../src/algorithm/graph/constraints/count_inconsistent_modulo_three_constraints.hpp
---

変数間の法3差分制約を入力順に追加し、それ以前に受理した制約と両立しない制約の個数を返す。

## ModularDifferenceConstraint

```cpp
struct ModularDifferenceConstraint {
    int difference;
    int left;
    int right;
};
```

制約

```text
value[left] - value[right] == difference (mod 3)
```

を表す。`difference` は法3で正規化される。

## count_inconsistent_modulo_three_constraints

```cpp
int count_inconsistent_modulo_three_constraints(
    int variable_count,
    const vector<ModularDifferenceConstraint>& constraints
);
```

各制約を先頭から処理する。両立する制約だけを以後の制約集合へ追加し、矛盾する制約と添字範囲外の制約を数える。

## API別の時間計算量・空間計算量

変数数を $N$、制約数を $K$、逆Ackermann関数を $\alpha$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_inconsistent_modulo_three_constraints` | $O((N+K)\alpha(N))$ | $O(N)$ |

## 注意点

- `variable_count` は非負でなければならない。違反時は `runtime_error` を送出する。
- `left` または `right` が $[0,N)$ の外にある制約は、例外ではなく矛盾する制約として数える。
- 同じ変数に0でない差を要求する制約は矛盾する。
