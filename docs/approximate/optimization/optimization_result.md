---
title: Optimization Result (連続最適化の返却値)
documentation_of: ../../../src/approximate/optimization/optimization_result.hpp
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

`point` と `value` は返すときの解とその目的関数値である。`iterations` は完了した反復回数、`evaluations` は目的関数の呼び出し回数、`gradient_evaluations` は勾配または偏微分の呼び出し回数を表す。`converged` は停止条件を満たしたときだけ真になり、反復回数の上限で停止した場合は偽である。

## 計算量

各メンバの参照は $O(1)$ である。次元を $D$ として、`point` のコピーは時間計算量・追加空間計算量ともに $O(D)$、ムーブは $O(1)$ である。

## 注意点

各メンバは公開されており、直接書き換えられる。書き換え時の整合性検査は行わない。反復回数・評価回数・収束判定の意味は、この値を生成したアルゴリズムに従う。
