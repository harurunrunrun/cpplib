---
title: Min-Conflicts for Finite-Domain CSP (有限領域制約充足の最小衝突法)
documentation_of: ../../../src/approximate/constraint/min_conflicts.hpp
---

有限領域の CSP（制約充足問題）に対する Min-Conflicts 法を `approximate::constraint` 名前空間で提供する。`domain_sizes[i]` は変数 $i$ の値域が `[0, domain_sizes[i])` であることを表す。変数が 0 個なら空の代入を解とし、サイズ 0 の値域があれば `std::invalid_argument` を送出する。

コールバック `conflict_count(variable, candidate_value, assignment)` は、変数を候補値に置き換えたときに、その変数に接続する違反制約の個数を非負整数で返す。合計は制約と変数の接続（incidence）の個数なので、二項制約は通常 2 回数えられるが、合計が 0 であることと全制約の充足が一致するようにする。

## `MinConflictsResult`

代入 `assignment`、衝突の接続数の合計 `conflict_incidence`、更新回数 `steps`、初期化回数 `restarts`、衝突が 0 であることを表す `found` を持つ。

## `is_valid_min_conflicts_result` / `validate_min_conflicts_result`

値域、衝突数の合計、`found` を再計算して検査する。前者は `bool` を返し、後者は不整合なら `std::invalid_argument` を送出する。

- 時間計算量: $O(VQ)$
- 追加空間計算量: $O(1)$

## `min_conflicts`

```cpp
auto result = min_conflicts(
    domain_sizes, conflict_count, rng,
    steps_per_restart, restart_limit, random_move_probability = 0.0
);
```

各再始動をランダムな代入から始める。衝突している変数を一様に選び、通常はその変数の局所的な衝突数を最小にする値（同数なら最小の値）へ変更する。確率 `random_move_probability` で、値を一様ランダムに選ぶ。確率 0 が通常版、正の値が Randomized Min-Conflicts である。

- 時間計算量: $O(VQ+R(VQ+S(V+D_{\max})Q))$
- 追加空間計算量: $O(V)$

## `randomized_min_conflicts`

ランダム移動の確率を必須引数にした、`min_conflicts` と同じ動作のラッパーである。時間計算量・追加空間計算量は `min_conflicts` と同じである。

ここで $V$ は変数数、$Q$ はコールバック 1 回のコスト、$R$ は再始動回数、$S$ は 1 回の再始動あたりのステップ数、$D_{\max}$ は最大の値域サイズである。

## 注意点

- コールバックの戻り値が負、`size_t` に収まらない、または合計がオーバーフローする場合は例外（`std::invalid_argument` または `std::overflow_error`）を送出する。
- `random_move_probability` は $[0,1]$ の値でなければならず、違反時は `std::invalid_argument` を送出する。
- `found == false` は指定した上限内で解が見つからなかったことだけを表し、制約系が充足不能であることの証明にはならない。
