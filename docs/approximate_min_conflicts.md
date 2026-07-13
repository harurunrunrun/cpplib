---
title: Min-Conflicts for Finite-Domain CSP (有限領域制約充足の最小衝突法)
documentation_of: ../src/approximate/constraint/min_conflicts.hpp
---

有限領域CSP用Min-Conflictsを `approximate::constraint` 名前空間で提供する。
`domain_sizes[i]` は変数 $i$ の値域 `[0, domain_sizes[i])` である。空の変数列
は解とし、サイズ0の値域は `std::invalid_argument` とする。

callback `conflict_count(variable, candidate_value, assignment)` は変数を候補値に
置換したとき、その変数に接続する違反制約数を非負整数で返す。合計は制約
incidenceなので二項制約を通常2回数えるが、0と全制約充足は一致させる。

# `MinConflictsResult`

`assignment`、`conflict_incidence`、更新数 `steps`、初期化数 `restarts`、
衝突0を示す `found` を保持する。負の戻り値、`size_t` に収まらない戻り値、
合計overflowには例外を送出する。

# `is_valid_min_conflicts_result` / `validate_min_conflicts_result`

値域、衝突合計、`found` を再計算する。前者はbool、後者は不整合なら
`std::invalid_argument` を送出する。

- 時間計算量: $O(VQ)$
- 追加空間計算量: $O(1)$

# `min_conflicts`

```cpp
auto result = min_conflicts(
    domain_sizes, conflict_count, rng,
    steps_per_restart, restart_limit, random_move_probability
);
```

各restartをランダム代入から始める。衝突変数を一様に選び、通常は局所衝突
最小値（同数なら値最小）へ変更する。指定確率では値を一様に選ぶ。確率0が
通常版、正値がRandomized Min-Conflictsである。

- 時間計算量: $O(VQ+R(VQ+S(V+D_{max})Q))$
- 追加空間計算量: $O(V)$

# `randomized_min_conflicts`

random move確率を必須にした `min_conflicts` の同値wrapperである。

- 時間計算量: `min_conflicts` と同じ
- 追加空間計算量: `min_conflicts` と同じ

$Q$ はcallback 1回、$R,S$ はrestart・step数、$D_{max}$ は最大値域である。
`found == false` は制約系が不能であることを証明しない。
