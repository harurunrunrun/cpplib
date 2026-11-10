---
title: Differential Evolution (差分進化法)
documentation_of: ../../../src/approximate/evolutionary/differential_evolution.hpp
---

箱型制約付きの連続最小化問題に DE/rand/1/bin（差分進化）を適用する。

## `differential_evolution`

```cpp
auto result = approximate::evolutionary::differential_evolution(
    population, lower, upper, evaluate, generations,
    differential_weight, crossover_probability, rng, compare = {}
);
```

個体は `std::vector<Real>` で、4 個体以上が必要である。各対象個体について、それとは異なる 3 個体 $a, b, c$ を選び、`a + differential_weight * (b - c)` と二項交叉して候補を作る。少なくとも 1 座標は必ず変異ベクトルから取る。候補は上下限へ切り詰め、対象個体より良いときだけ次世代に残す。世代の更新は同期的に行う。

`Compare` のデフォルトは `std::less<>` であり、変更すれば最大化もできる。乱数生成器は参照で渡す。戻り値の型は `EvolutionResult<std::vector<Real>, Score>` である。

## 計算量

集団サイズを $N$、次元を $D$、世代数を $G$、目的関数 1 回の時間を $E$ とすると、時間計算量は $O(NE+GN(D+E))$、追加空間計算量は $O(ND+N)$ である。

## 注意点

- `differential_weight` は非負の有限値、`crossover_probability` は $[0,1]$ でなければならない。これらの違反、個体数が 4 未満、次元の不一致、空の個体、箱の外にある初期個体では `std::invalid_argument` を送出する。
- 目的関数は呼び出し中に有効でなければならず、比較関数は一貫した順序を与えなければならない。
- 結果の再現には、同じ乱数生成器の状態・入力・標準ライブラリの実装が必要である。大域最適性は保証しない。
