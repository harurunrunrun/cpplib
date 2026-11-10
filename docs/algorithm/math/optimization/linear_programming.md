---
title: Linear Programming (Two-phase Simplex) (線形計画法)
documentation_of: ../../../../src/algorithm/math/optimization/linear_programming.hpp
---

二段階単体法（simplex 法）で、次の標準形の線形計画問題を解く。

$$
\text{maximize/minimize } c^T x,\qquad Ax\le b,\qquad x\ge0.
$$

係数とタブローには `long double` を用いる。容量をコンパイル時に固定する固定長版と、実行時のサイズに合わせて `std::vector` を使う可変長版がある（いずれもグローバル名前空間）。

```cpp
LinearProgramming<MAX_CONSTRAINT, MAX_VARIABLE> problem(variable_count);  // 固定長版
LinearProgramming<> dynamic(variable_count);                              // 可変長版
dynamic.set_objective(std::vector<long double>(variable_count, 1));
```

固定長版では `variable_count` は $[0, \texttt{MAX_VARIABLE}]$ の範囲でなければならない。オブジェクト本体に係数行列を持つので、容量が大きい場合は静的領域またはヒープ上に置く。

## 結果

```cpp
enum LinearProgrammingStatus {
    LINEAR_PROGRAMMING_OPTIMAL,
    LINEAR_PROGRAMMING_INFEASIBLE,
    LINEAR_PROGRAMMING_UNBOUNDED,
};

template<size_t MAX_VARIABLE>
struct LinearProgrammingResult {
    LinearProgrammingStatus status;
    long double value;
    std::array<long double, MAX_VARIABLE> solution;  // 可変長版では std::vector<long double>
    int variable_count;
};
```

- `LINEAR_PROGRAMMING_OPTIMAL`: 有限の最適値を持つ。`value` と `solution` の先頭 `variable_count` 個が有効。
- `LINEAR_PROGRAMMING_INFEASIBLE`: 制約をすべて満たす点がない。
- `LINEAR_PROGRAMMING_UNBOUNDED`: 実行可能だが、指定した向きに目的関数値の上限（最小化では下限）がない。

固定長版の `solution` の変数個数を超える部分は $0$ である。各メンバの参照は $O(1)$ 時間・空間。

## 状態の参照

```cpp
problem.variable_count()
problem.constraint_count()
```

変数の個数と、追加済みの制約の本数を返す。どちらも $O(1)$ 時間・空間。

## 目的関数

```cpp
problem.set_objective(coefficients)
```

固定長版では `std::array<long double, MAX_VARIABLE>` の先頭 `variable_count` 個を、可変長版では長さ `variable_count` の `std::vector<long double>` を目的関数の係数 $c$ として設定する。再設定してもよい。

- 時間計算量: $O(\texttt{MAX_VARIABLE})$（可変長版では $O(N)$）
- 追加空間: $O(1)$

## 制約の追加

```cpp
problem.add_less_equal(coefficients, bound)
problem.add_greater_equal(coefficients, bound)
problem.add_equal(coefficients, bound)
```

`add_less_equal` は $a^T x \le \mathrm{bound}$、`add_greater_equal` は $a^T x \ge \mathrm{bound}$ を 1 本追加する。`add_equal` は両向きの不等式を 2 本追加するので、制約の容量を 2 消費する。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `add_less_equal` | $O(\texttt{MAX_VARIABLE})$ | $O(1)$ |
| `add_greater_equal` | $O(\texttt{MAX_VARIABLE})$ | $O(\texttt{MAX_VARIABLE})$ |
| `add_equal` | $O(\texttt{MAX_VARIABLE})$ | $O(\texttt{MAX_VARIABLE})$ |

可変長版では $\texttt{MAX_VARIABLE}$ を変数の個数 $N$ に読み替える。固定長版で容量を超えると `std::runtime_error` を送出する。係数または右辺に NaN や無限大があると `std::invalid_argument` を送出する。可変長版で係数の長さが変数の個数と異なる場合も `std::invalid_argument` を送出する。失敗時には制約は追加されない。

## 最大化・最小化

```cpp
problem.maximize()
problem.minimize()
```

現在の目的関数と制約で問題を解き、`LinearProgrammingResult` を返す。`minimize` は目的関数の符号を反転して同じタブローを解く。どちらも `problem` を変更しないので、同じ問題を繰り返し解ける。

Phase I で人工変数を使って初期の実行可能基底を求め、Phase II で目的関数を最適化する。入る変数・出る変数の候補が同率のときは変数番号で選び、循環を避ける。

$M$ を `constraint_count()`、$N$ を `variable_count()`、ピボットの回数を $P$ とすると、1 回のピボットは $O(MN)$ 時間である。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `maximize` | $O(\texttt{MAX_CONSTRAINT} \cdot \texttt{MAX_VARIABLE} + PMN)$ | $O(\texttt{MAX_CONSTRAINT} \cdot \texttt{MAX_VARIABLE})$ |
| `minimize` | $O(\texttt{MAX_CONSTRAINT} \cdot \texttt{MAX_VARIABLE} + PMN)$ | $O(\texttt{MAX_CONSTRAINT} \cdot \texttt{MAX_VARIABLE})$ |

可変長版では時間 $O(MN + PMN)$、作業領域 $O(MN)$ になる。単体法のピボット回数には一般に多項式の上界がなく、最悪時間は指数的である。通常の（病的でない）競技プログラミングの入力を対象とする。

## 標準形への変換

- $a^T x \ge b$ と等式は、対応する API が変換する。
- 上限 $x_i \le u$ は、$i$ 番目の係数だけが $1$ の制約として追加する。
- 下限 $x_i \ge l$ は `add_greater_equal` で追加する。
- 符号に制約のない変数 $y$ は、$y = y^{+} - y^{-}$ として非負の変数 2 個に分ける。
- 定数項を含む目的関数は、解いた後にその定数を `value` に加える。

## 注意点

- 内部の判定の許容誤差は `LinearProgramming::EPS = 1e-12L` である。係数のスケールの差が大きい問題、退化が強い問題、厳密解が必要な問題では誤判定や誤差が起こりうる。行や変数のスケールを揃え、必要なら有理数による厳密な LP を使う。
- 固定長版はオブジェクト構築時に固定長配列を $0$ で初期化するので、$O(\texttt{MAX_CONSTRAINT} \cdot \texttt{MAX_VARIABLE})$ 時間・空間かかる。可変長版の保存領域は $O(MN)$。
- 入力とタブローの中間値はすべて有限の `long double` に収まる必要がある。
