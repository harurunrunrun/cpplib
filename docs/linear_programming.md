---
title: Linear Programming (Two-phase Simplex)
documentation_of: ../src/algorithm/math/linear_programming.hpp
---

二段階simplex法で、次の標準形の線形計画問題を解く。

\[
\text{maximize/minimize } c^T x,\qquad Ax\le b,\qquad x\ge0.
\]

係数とtableauには `long double` を用い、容量はcompile時に固定する。

~~~cpp
LinearProgramming<MAX_CONSTRAINT, MAX_VARIABLE> problem(variable_count);
~~~

`variable_count` は `[0, MAX_VARIABLE]`。object本体に係数行列を持つため、大容量ではstatic storageまたはheap上へ置く。

# 結果

~~~cpp
enum LinearProgrammingStatus {
    LINEAR_PROGRAMMING_OPTIMAL,
    LINEAR_PROGRAMMING_INFEASIBLE,
    LINEAR_PROGRAMMING_UNBOUNDED,
};

template<size_t MAX_VARIABLE>
struct LinearProgrammingResult {
    LinearProgrammingStatus status;
    long double value;
    array<long double, MAX_VARIABLE> solution;
    int variable_count;
};
~~~

- `OPTIMAL`: 有限な最適値を持つ。`value` と先頭 `variable_count` 個の `solution` が有効。
- `INFEASIBLE`: 制約をすべて満たす点がない。
- `UNBOUNDED`: 実行可能で、指定した向きの目的値に上限または下限がない。

`solution` の容量外部分は0。各field参照は `O(1)` 時間・空間。

# 状態の参照

~~~cpp
problem.variable_count()
problem.constraint_count()
~~~

変数数と追加済み制約数を返す。ともに `O(1)` 時間・空間。

# 目的関数

~~~cpp
problem.set_objective(coefficients)
~~~

`std::array<long double, MAX_VARIABLE>` の先頭 `variable_count` 個を目的関数 `c` として設定する。再設定可能。

- 時間計算量: `O(MAX_VARIABLE)`
- 追加空間: `O(1)`

# 制約の追加

~~~cpp
problem.add_less_equal(coefficients, bound)
problem.add_greater_equal(coefficients, bound)
problem.add_equal(coefficients, bound)
~~~

`add_less_equal` は `a^T x <= bound`、`add_greater_equal` は `a^T x >= bound` を1本追加する。`add_equal` は両向きの不等式2本を追加するため、constraint容量を2消費する。

| API | 時間計算量 | 追加空間 |
| --- | --- | --- |
| `add_less_equal` | `O(MAX_VARIABLE)` | `O(1)` |
| `add_greater_equal` | `O(MAX_VARIABLE)` | `O(MAX_VARIABLE)` |
| `add_equal` | `O(MAX_VARIABLE)` | `O(MAX_VARIABLE)` |

容量超過では `runtime_error`。係数または右辺にNaN/Infinityがあれば `invalid_argument`。失敗時に制約は追加されない。

# 最大化・最小化

~~~cpp
problem.maximize()
problem.minimize()
~~~

現在の目的関数と制約を解き、`LinearProgrammingResult` を返す。`minimize` は目的関数の符号を反転して同じtableauを解く。どちらも `problem` を変更せず、同じ問題を繰り返し解ける。

Phase I で人工変数を使って初期実行可能基底を求め、Phase II で目的関数を最適化する。entering/leaving variableの同率時は変数番号で選び、循環を避ける。

`M=constraint_count()`、`N=variable_count()`、pivot回数を `P` とすると、1回のpivotは `O(MN)` 時間である。

| API | 時間計算量 | 追加空間 |
| --- | --- | --- |
| `maximize` | `O(MAX_CONSTRAINT * MAX_VARIABLE + P M N)` | `O(MAX_CONSTRAINT * MAX_VARIABLE)` |
| `minimize` | `O(MAX_CONSTRAINT * MAX_VARIABLE + P M N)` | `O(MAX_CONSTRAINT * MAX_VARIABLE)` |

simplex法のpivot回数には一般の多項式上界がなく、最悪時間は指数的である。通常の非病的な競技プログラミング入力を対象とする。

# 標準形への変換

- `a^T x >= b` と等式は対応APIが変換する。
- 上限 `x_i <= u` は係数が `i` だけ1の制約として追加する。
- 下限 `x_i >= l` は `add_greater_equal` で追加する。
- 符号自由な変数 `y` は `y = y_positive - y_negative` として非負変数2個へ分ける。
- 定数項を含む目的関数は、解いた後にその定数を `value` へ加える。

# 数値上の前提

内部判定幅は `LinearProgramming::EPS = 1e-12L`。係数のscale差が大きい問題、退化が強い問題、厳密解が必要な問題では誤判定や誤差が起こり得る。行・変数のscaleを揃え、必要なら有理数またはexact LPを使う。

object構築時は固定配列を0初期化するため `O(MAX_CONSTRAINT * MAX_VARIABLE)` 時間・空間。入力とtableauの全中間値が有限な `long double` に収まる必要がある。
