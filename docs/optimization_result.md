---
title: Optimization Result (連続最適化の返却値)
documentation_of: ../src/approximate/optimization/optimization_result.hpp
---

## `OptimizationResult`

```cpp
template<class Real>
struct OptimizationResult{
    std::vector<Real> point;
    Real value;
    std::size_t iterations;
    std::size_t evaluations;
    std::size_t gradient_evaluations;
    bool converged;
};
```

`point`と`value`は返却時の解と目的関数値である。`iterations`は完了した反復数、
`evaluations`は目的関数の呼び出し回数、`gradient_evaluations`は勾配または
座標微分の呼び出し回数を表す。停止条件を満たしたときだけ`converged`が真になる。
反復数上限で停止した場合は偽である。

## 計算量

各メンバの参照は$O(1)$である。`point`のコピーは次元を$D$として時間計算量・追加空間計算量ともに
$O(D)$、ムーブは$O(1)$である。

## 注意点

各メンバは公開されており直接書き換えられる。書き換え時の整合性検査は行われず、反復数・評価回数・収束判定の意味はこの値を生成したアルゴリズムに従う。
