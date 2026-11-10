---
title: CNF Formula (連言標準形論理式)
documentation_of: ../../../src/approximate/constraint/cnf.hpp
---

`approximate::constraint` 名前空間で、局所探索用の CNF（連言標準形）論理式を表す。リテラル `i` は変数 `i-1`、`-i` はその否定を表し、代入は `std::vector<unsigned char>` の 0/1 列である。節が 0 個の式は真、空節を含む式は偽とする。重複したリテラルや恒真な節もそのまま保持する。

以下、変数数を $V$、節数を $C$、全リテラル数を $L$、節 $i$ の長さを $\lvert C_i \rvert$ とする。

## API

### `CnfLiteral`, `BooleanAssignment`, `CnfFormula::Clause`

`CnfLiteral` は `std::int64_t`、`BooleanAssignment` は `std::vector<unsigned char>`、`Clause` は `std::vector<CnfLiteral>` の別名である。別名自体に実行時のコストはない。

### `CnfFormula()`

変数も節もない真の論理式を作る。時間計算量・追加空間計算量はともに $O(1)$ である。

### `CnfFormula(variable_count, clauses)`

変数数と節の列を保持する。リテラルが 0 または変数の範囲外（`INT64_MIN` を含む）なら `std::invalid_argument` を送出する。

- 時間計算量: $O(L)$
- 追加空間計算量: $O(L+C)$（保持する節の列を含む）

### `variable_count()` / `clause_count()` / `clauses()`

変数数、節数、節の列への const 参照を返す。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

### `clause_satisfied(clause_index, assignment)`

指定した節が満たされるかを判定する。代入と節番号も検査する。

- 時間計算量: $O(V+\lvert C_i \rvert)$
- 追加空間計算量: $O(1)$

### `satisfied_clause_count(assignment)`

満たされた節の個数を返す。代入の長さと値も検査する。

- 時間計算量: $O(V+L)$
- 追加空間計算量: $O(1)$

### `satisfied(assignment)`

全ての節が満たされるかを判定する。

- 時間計算量: $O(V+L)$
- 追加空間計算量: $O(1)$

### `unsatisfied_clause_indices(assignment)`

満たされない節の番号を昇順で返す。

- 時間計算量: $O(V+L)$
- 追加空間計算量: $O(C)$

## 注意点

- 代入は長さ $V$ の 0/1 列でなければならず、違反時は `std::invalid_argument` を送出する。
- 0 や変数の範囲外のリテラルは `std::invalid_argument`、範囲外の節番号は `std::out_of_range` となる。
