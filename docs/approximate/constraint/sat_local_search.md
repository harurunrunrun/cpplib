---
title: GSAT / WalkSAT / ProbSAT (SAT局所探索)
documentation_of: ../../../src/approximate/constraint/sat_local_search.hpp
---

`approximate::constraint` 名前空間で、CNF-SAT に対する確率的局所探索を提供する。乱数生成器を参照で明示的に受け取り、1 回の再始動あたりの反転（flip）回数の上限と再始動回数を指定する。同じ型・状態の乱数生成器を使えば結果は再現できる。

以下、変数数を $V$、節数を $C$、全リテラル数を $L$、再始動回数を $R$、1 回の再始動あたりのステップ数を $S$、選んだ節に含まれる相異なる変数の個数を $K$ とする。

## `SatSearchResult`

最良の代入 `assignment`、その充足節数 `satisfied_clauses`、反転の総数 `steps`、開始した初期化の回数 `restarts` を持つ。`found` は返した代入が全ての節を満たす場合に限り `true` になる。

## `is_valid_sat_search_result` / `validate_sat_search_result`

代入、充足節数、`found` の整合性を再計算して検査する。前者は `bool` を返し、後者は不整合なら `std::invalid_argument` を送出する。

- 時間計算量: $O(V+L)$
- 追加空間計算量: $O(1)$

## `gsat`

```cpp
auto result = gsat(formula, rng, steps_per_restart, restart_limit);
```

各ステップで全ての反転を評価し、充足節数が最大になるものを選ぶ。同数なら変数番号が最小のものを選ぶ。

- 時間計算量: $O(V+L+R(V+L+SV(V+L)))$
- 追加空間計算量: $O(V)$

## `walksat`

```cpp
auto result = walksat(formula, rng, steps_per_restart, restart_limit,
                      probability = 0.5);
```

未充足節を一様に選ぶ。確率 `probability` で節内の相異なる変数から一様に選び、それ以外では break 数（反転で新たに偽になる節の数）が最小の変数（同数なら変数番号が最小）を選ぶ。

- 時間計算量: $O(V+L+R(V+L+S(V+KL)))$
- 追加空間計算量: $O(V+C+K)$

## `probsat`

```cpp
auto result = probsat(formula, rng, steps_per_restart, restart_limit,
                      base = 2.3);
```

未充足節の変数 $x$ を、$b^{-(\operatorname{break}(x)-\min\operatorname{break})}$ に比例する確率で選んで反転する（$b$ は `base`）。最小の break 数を持つ変数の重みを 1 に正規化することで、全ての重みが同時にアンダーフローするのを防ぐ。

- 時間計算量: $O(V+L+R(V+L+S(V+KL)))$
- 追加空間計算量: $O(V+C+K)$

## 注意点

- `walksat` の確率は $[0,1]$ の有限値、`probsat` の $b$ は 1 より大きい有限値でなければならず、違反時は `std::invalid_argument` を送出する。
- 空節があれば探索せずに失敗結果を返す。`restart_limit` が 0 なら全て 0 の代入を評価した結果だけを返す。
- カウンタがオーバーフローする直前には `std::overflow_error` を送出する。
- `found == false` は上限内で解が見つからなかったという意味であり、充足不能の証明ではない。
