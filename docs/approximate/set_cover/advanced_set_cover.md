---
title: Advanced Set Cover and Rounding (高度な集合被覆と丸め)
documentation_of: ../../../src/approximate/set_cover/advanced_set_cover.hpp
---

名前空間 `approximate::set_cover` に、重み付き集合被覆問題に対する主双対法（primal-dual）・局所比法（local ratio）と、分数解（LP 緩和解）の丸め手法を提供する。

以下、宇宙の大きさを $U$（`universe_size`）、集合数を $S$、正規化（各集合のソート・重複除去）後の所属関係の総数を $L$、要素の最大出現頻度（その要素を含む集合の最大個数）を $f$ とする。

## 主双対法・局所比法

```cpp
PrimalDualSetCoverResult primal_dual_set_cover(universe_size, sets, weights);
SetCoverResult local_ratio_set_cover(universe_size, sets, weights);
```

- `primal_dual_set_cover`: 要素を添字の昇順に見て、未被覆要素の双対変数を、その要素を含む集合のいずれかが tight（残余重み 0）になるまで増やし、tight になった集合のうち添字最小のものを選ぶ。戻り値 `PrimalDualSetCoverResult` はメンバ `cover`（`SetCoverResult`）と `element_dual`（各要素の双対変数の値、`std::vector<long double>`）を持つ。
- `local_ratio_set_cover`: 未被覆要素を含む集合の残余重みからその最小値を引き、重み 0 になった集合のうち添字最小のものを選ぶ。

両者とも最後に reverse-delete（選択の逆順に、取り除いても被覆が保たれる集合を除く）で冗長な集合を取り除く。どの集合にも含まれない要素は `uncovered_elements` に残る。

正規化を含む時間計算量は $O(L\log L)$、空間計算量は $O(U+S+L)$。近似比は $f$ である。

## 集合被覆の丸め

```cpp
randomized_rounding_set_cover(universe_size, sets, fractional, random, rounds = 1);
deterministic_rounding_set_cover(universe_size, sets, fractional);
iterative_rounding_set_cover(universe_size, sets, fractional);
lp_rounding_set_cover(universe_size, sets, fractional);
```

`fractional` は各集合の分数解 $x_i$（`std::vector<long double>`）、`random` は一様乱数生成器である。戻り値はいずれも `SetCoverResult`。

- `randomized_rounding_set_cover`: 各ラウンドで未選択の集合 $i$ を確率 $x_i$ で独立に選ぶ。`rounds` ラウンド繰り返す。未被覆要素が残る場合がある。
- `deterministic_rounding_set_cover`: 実行可能な LP 解を閾値 $1/f$ で丸める（$x_i \ge 1/f$ の集合を選ぶ）。
- `lp_rounding_set_cover`: `deterministic_rounding_set_cover` と同じ。
- `iterative_rounding_set_cover`: 添字最小の未被覆要素を取り、それを含む集合のうち $x_i$ が最大（同値なら添字最小）のものを選ぶことを繰り返す。

時間計算量はラウンド数を $R$ として、乱択版が $O(RS+L\log L)$、決定的丸め・LP 丸めが $O(S+L\log L)$、反復丸めが最悪 $O(U\min(U,S)+L\log L)$。空間計算量はいずれも $O(U+S+L)$。

## 従属丸め・Pipage 丸め

```cpp
std::vector<unsigned char> dependent_rounding(fractional, random);
std::vector<unsigned char> pipage_rounding(fractional, objective);
```

- `dependent_rounding`: 分数値の変数を 2 つ選び、和と各変数の期待値を保つ二択の更新を確率的に行って 0/1 に近づける。最後に分数値の変数が 1 つだけ残った場合は、その値を確率として独立に丸める。
- `pipage_rounding`: 同じく和を保つ方向の 2 つの端点を `objective(vector)` で評価し、値が大きい側を決定的に選ぶ。分数値の変数が 1 つだけ残った場合は 0 と 1 の両方を評価する。

変数数を $S$、`objective` の 1 回の評価時間を $Q$ とすると、時間計算量は `dependent_rounding` が $O(S^2)$、`pipage_rounding` が $O(S^2+SQ)$。空間計算量は $O(S)$。

## 注意点

- 集合の要素は $[0, U)$ になければならない。範囲外なら `std::out_of_range` を投げる。
- `weights` は集合と同数で、非負かつ有限でなければならない。`Weight` は `bool` 以外の算術型。違反すると `std::invalid_argument` を投げる。
- `fractional` は集合と同数で、各値は有限かつ $[0,1]$ になければならない（違反時は `std::invalid_argument`）。
- 決定的丸め・反復丸め・LP 丸めは、各要素 $e$ について $\sum_{i:e\in S_i}x_i\ge1$ を満たす実行可能解を要求し、満たさなければ `std::invalid_argument` を投げる（どの集合にも含まれない要素があるときも同様）。比較には小さな許容誤差を入れている。
- 乱択丸めの被覆は確率的であり、ラウンド数を増やしても完全被覆は保証されない。
- Pipage 丸めの近似保証は、渡した `objective` が必要な凸性条件を満たす場合に限る。
