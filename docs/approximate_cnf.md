---
title: CNF Formula (連言標準形論理式)
documentation_of: ../src/approximate/constraint/cnf.hpp
---

`approximate::constraint` 名前空間で局所探索用CNFを表す。リテラル `i` は
変数 `i-1`、`-i` はその否定で、代入は `vector<unsigned char>` の0/1列で
ある。節0個は真、空節を含む式は偽とする。重複リテラルと恒真節も保持する。

# `CnfFormula(variable_count, clauses)`

変数数と節列を保持する。リテラル0、範囲外（`INT64_MIN` を含む）には
`std::invalid_argument` を送出する。$V,C,L$ を変数・節・全リテラル数とする。

- 時間計算量: $O(L)$
- 追加空間計算量: $O(L+C)$（保持する節列を含む）

# `variable_count()` / `clause_count()` / `clauses()`

変数数、節数、節列へのconst参照を返す。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

# `clause_satisfied(clause_index, assignment)`

指定節が満たされるか判定し、代入と節番号を検査する。

- 時間計算量: $O(V+|C_i|)$
- 追加空間計算量: $O(1)$

# `satisfied_clause_count(assignment)`

満たされた節数を返し、代入の長さ・値を検査する。

- 時間計算量: $O(V+L)$
- 追加空間計算量: $O(1)$

# `satisfied(assignment)`

全節が満たされるか判定する。

- 時間計算量: $O(V+L)$
- 追加空間計算量: $O(1)$

# `unsatisfied_clause_indices(assignment)`

未充足節番号を昇順で返す。

- 時間計算量: $O(V+L)$
- 追加空間計算量: $O(C)$
