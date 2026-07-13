---
title: Evolution Result (進化計算の結果)
documentation_of: ../src/approximate/evolutionary/evolution_result.hpp
---

`approximate::evolutionary`名前空間の進化計算で共通に使う結果型を提供する。

## `EvolutionResult`

```cpp
template<class Individual, class Score>
struct EvolutionResult{
    Individual individual;
    Score score;
    std::size_t iterations;
    std::size_t evaluations;
};
```

- `individual`: 探索中に得た最良個体。
- `score`: `individual`の目的値。
- `iterations`: 実行した世代数または反復数。
- `evaluations`: 目的関数を呼んだ回数。

各メンバへのアクセスは時間計算量・追加空間計算量ともに$O(1)$である。
`individual`または`score`をコピーする場合は、それぞれの型のコピー費用がかかる。

## 注意点

各メンバは公開されており直接書き換えられる。書き換え時の整合性検査は行われず、`iterations` と `evaluations` の意味はこの値を生成したアルゴリズムに従う。
