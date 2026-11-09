---
title: Inconsistent Modulo-Three Constraints Count (法3差分制約の矛盾数) [CHAIN]
documentation_of: ../../../../../src/algorithm/graph/constraints/systems/count_inconsistent_modulo_three_constraints.hpp
---

変数間の法 3 の差分制約を入力順に追加していき、それ以前に受理した制約と両立しない制約の個数を返す。重み付き Union-Find を用いる。

## `ModularDifferenceConstraint`

```cpp
struct ModularDifferenceConstraint {
    int difference;
    int left;
    int right;
};
```

制約

$$
\mathrm{value}[\mathrm{left}] - \mathrm{value}[\mathrm{right}] \equiv \mathrm{difference} \pmod 3
$$

を表す。`difference` は負でもよく、法 3 で正規化して扱う。

## `count_inconsistent_modulo_three_constraints`

```cpp
int count_inconsistent_modulo_three_constraints(
    int variable_count,
    const std::vector<ModularDifferenceConstraint>& constraints
);
```

制約を先頭から順に処理する。両立する制約だけを以後の制約集合へ追加し、矛盾する制約と添字が範囲外の制約の個数を返す。

## 計算量

変数の個数を $N$、制約の個数を $K$、逆アッカーマン関数を $\alpha$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_inconsistent_modulo_three_constraints` | $O(N + K \alpha(N))$ | $O(N)$ |

## 注意点

- `variable_count` は非負でなければならない。違反時は `runtime_error` を送出する。
- `left` または `right` が $[0, N)$ の範囲外である制約は、例外を送出せず矛盾する制約として数える。
- 同じ変数どうしに $0$ 以外の差を要求する制約は矛盾する。
