---
title: Compact Genetic Algorithm (コンパクト遺伝的アルゴリズム)
documentation_of: ../../../src/approximate/evolutionary/compact_genetic_algorithm.hpp
---

独立なベルヌーイ分布で表した仮想的な集団を使って二値列を最適化する、compact GA を提供する。

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

探索中の最良の二値列、その目的値、各ビットが 1 になる最終的な確率、反復回数、評価回数を持つ。スカラーのメンバへのアクセスは $O(1)$、二値列または確率列のコピーは $O(B)$ である。

## `compact_genetic_algorithm`

```cpp
auto result = approximate::evolutionary::
    compact_genetic_algorithm<Real>(
        bit_count, evaluate, iterations, virtual_population_size,
        rng, compare = {}, initial_probability = 0.5
    );
```

各反復で確率ベクトルから 2 個体を生成し、良い方の個体のビットへ確率を `1 / virtual_population_size` だけ近づける。確率は $[1/N, 1-1/N]$（$N$ は仮想集団サイズ）に収める。`evaluate(bits)` が目的値を返す。`Compare` のデフォルトは `std::less<>`、初期確率のデフォルトは 0.5 である。乱数生成器は参照で渡す。

## 計算量

ビット数を $B$、反復回数を $I$、目的関数 1 回の時間を $E$ とすると、時間計算量は $O(I(B+E))$、追加空間計算量は $O(B)$ である。目的関数はちょうど $2I$ 回呼ばれる。

## 注意点

- `bit_count` と `iterations` は正、仮想集団サイズは 2 以上、初期確率は $[0,1]$ でなければならない。違反時は `std::invalid_argument` を送出する。
- 目的関数は呼び出し中に有効でなければならず、比較関数は一貫した順序を与えなければならない。
- 結果の再現には、同じ乱数生成器の状態・入力・標準ライブラリの実装が必要である。大域最適性は保証しない。
