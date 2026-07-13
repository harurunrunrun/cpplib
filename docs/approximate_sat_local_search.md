---
title: GSAT, WalkSAT, and ProbSAT
documentation_of: ../src/approximate/constraint/sat_local_search.hpp
---

`approximate::constraint` 名前空間でCNF-SATの確率的局所探索を提供する。
乱数生成器を参照で明示的に受け、1 restart当たりのflip上限とrestart数を
指定する。`found == false` は上限内で未発見という意味で充足不能の証明では
ない。同じ型・状態のURBGで再現できる。

# `SatSearchResult`

最良 `assignment`、`satisfied_clauses`、flip総数 `steps`、開始した初期化数
`restarts` を保持する。`found` は返却代入が全節を満たす場合だけ真になる。
カウンタoverflowの直前には `std::overflow_error` を送出する。

# `is_valid_sat_search_result` / `validate_sat_search_result`

代入、充足節数、`found` の整合性を再計算する。前者はbool、後者は不整合
なら `std::invalid_argument` を送出する。

- 時間計算量: $O(V+L)$
- 追加空間計算量: $O(1)$

# `gsat`

```cpp
auto result = gsat(formula, rng, steps_per_restart, restart_limit);
```

各stepで全flipを評価して最大のものを選ぶ。同数なら変数番号最小を選ぶ。

- 時間計算量: $O(V+L+R(V+L+SV(V+L)))$
- 追加空間計算量: $O(V)$

# `walksat`

```cpp
auto result = walksat(formula, rng, steps_per_restart, restart_limit, probability);
```

未充足節を一様に選ぶ。指定確率で節内の異なる変数を一様に選び、残りでは
break count最小（同数なら変数番号最小）を選ぶ。確率は有限な $[0,1]$ とする。

- 時間計算量: $O(V+L+R(V+L+S(V+KL)))$
- 追加空間計算量: $O(V+C+K)$

# `probsat`

```cpp
auto result = probsat(formula, rng, steps_per_restart, restart_limit, base);
```

未充足節の変数 $x$ を
$b^{-(\operatorname{break}(x)-\min\operatorname{break})}$ に比例してflipする。
$b$ は有限かつ1より大きい。最小breakを重み1に正規化して全重みの同時
underflowを防ぐ。

- 時間計算量: $O(V+L+R(V+L+S(V+KL)))$
- 追加空間計算量: $O(V+C+K)$

$V,C,L,R,S$ は変数・節・リテラル・restart・step数、$K$ は選択節の異なる
変数数である。空節があれば探索せず失敗結果を送出する。restartが0なら全0代入
の評価だけを送出する。
