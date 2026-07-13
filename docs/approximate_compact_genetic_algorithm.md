---
title: Compact Genetic Algorithm (コンパクト遺伝的アルゴリズム)
documentation_of: ../src/approximate/evolutionary/compact_genetic_algorithm.hpp
---

独立Bernoulli分布で表した仮想集団を使い、二値列を最適化するcompact GAを提供する。

## `CompactGeneticAlgorithmResult`

```cpp
template<class Real, class Score>
struct CompactGeneticAlgorithmResult{
    std::vector<std::uint8_t> individual;
    Score score;
    std::vector<Real> probabilities;
    std::size_t iterations;
    std::size_t evaluations;
};
```

探索中の最良二値列、その目的値、各bitが1となる最終確率、反復数、評価回数を持つ。
scalarメンバへのアクセスは$O(1)$、二値列または確率列のコピーは$O(B)$である。

## `compact_genetic_algorithm`

```cpp
auto result = approximate::evolutionary::
    compact_genetic_algorithm<Real>(
        bit_count, evaluate, iterations, virtual_population_size,
        rng, compare, initial_probability
    );
```

各反復で確率ベクトルから2個体を生成し、良い個体のbitへ確率を
`1 / virtual_population_size`だけ近づける。確率は
`[1/N, 1-1/N]`へclampする。`evaluate(bits)`が目的値を返す。
`Compare`の既定値は`std::less<>`、初期確率の既定値は0.5である。

`bit_count`と`iterations`は正、仮想集団サイズは2以上、初期確率は`[0, 1]`
でなければならない。違反時は`std::invalid_argument`を送出する。
乱数生成器は参照で渡す。

## 計算量

bit数を$B$、反復数を$I$、目的関数1回の時間を$E$とすると、時間計算量は
$O(I(B+E))$、追加空間計算量は$O(B)$である。目的関数は正確に$2I$回呼ばれる。

## 注意点

目的関数と遺伝的操作は呼び出し中に有効で、比較関数は一貫した順序を与える必要がある。乱数結果の再現性には同じ乱数器状態・入力・標準ライブラリ実装が必要で、大域最適性は保証しない。
