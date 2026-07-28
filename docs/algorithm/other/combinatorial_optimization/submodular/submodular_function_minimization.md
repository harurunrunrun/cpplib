---
title: Submodular Function Minimization (劣モジュラ関数最小化)
documentation_of: ../../../../../src/algorithm/other/combinatorial_optimization/submodular/submodular_function_minimization.hpp
---

value oracleで与えた劣モジュラ集合関数を、Wolfeのminimum-norm point法で最小化する。
得られたbase polytope上の点に対して厳密な下界証明を検査し、証明できた場合は
全部分集合を調べずに終了する。証明できなかった場合だけ全部分集合との照合へ
フォールバックするため、返す答えは常に厳密である。

## `SubmodularMinimizationResult<Value>`

`elements` と `value` は厳密な最小集合とoracle値である。`minimum_norm_point` は
base polytope上の候補点、`iterations` はWolfe反復数、`dual_gap` は停止時gap、
`minimum_norm_converged` はその数値反復がgap条件で収束したかを表す。
`exact_certificate_used` はbase polytopeの下界証明によって全列挙を省略したかを表す。

## `submodular_function_minimization(ground_set_size, oracle, maximum_iterations=10000, tolerance=1e-12)`

`oracle(set)` は長さ $n$ の0/1 vectorを受け取る。active base数を $a$、実行した
Wolfe反復数を $I$、oracle 1回の時間を $T$ とする。minimum-norm pointの候補生成は
$O(I(nT+n\log n+a^4+a^2n))$ 時間、$O(an+a^2+n)$ 空間である。

候補点順の1本のgreedy baseについて、負の限界差分の後に非負の限界差分が続く場合、
その符号とtightなprefixから厳密な最適性を証明する。整数値oracleではさらに、active
baseの係数を分母 $2^{48}$ の非負有理数へ丸め、和を正確に1に保ったbase polytope上の
点について、負座標の和が候補値と一致するかを `__int128` で検査する。これらの証明に
成功した場合の追加時間は $O(nT+an)$ で、指数時間部分はない。

証明に失敗した場合だけ $2^n$ 個を照合する。この場合を含む最悪時間計算量は
$O(I(nT+n\log n+a^4+a^2n)+2^n(n+T))$、最悪空間計算量は
$O(an+a^2+n)$ である。

## 注意点

oracle値は有限な数値へ変換でき、関数は劣モジュラでなければならない。
minimum-norm計算は `long double` だが、`elements` と `value` の厳密性には影響しない。
整数値証明は64 bit以下の組み込み整数型を対象とし、中間積には `__int128` を使う。
停止条件が不正なら `std::invalid_argument`、active setが数値的に特異なら
`std::runtime_error` を送出する。最適性証明が成立しない入力では全部分集合を評価するため、
大きい $n$ で常に高速に終了する保証はない。
