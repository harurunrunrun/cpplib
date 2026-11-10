---
title: Submodular Function Minimization (劣モジュラ関数最小化)
documentation_of: ../../../../../src/algorithm/other/combinatorial_optimization/submodular/submodular_function_minimization.hpp
---

値オラクルで与えた劣モジュラ集合関数を、Wolfe の最小ノルム点法（minimum-norm point algorithm）で最小化する。名前空間は `combinatorial_optimization` である。
得られた基多面体（base polytope）上の点について厳密な下界の証明を検査し、証明できた場合は全部分集合を調べずに終了する。証明できなかった場合だけ全部分集合との照合にフォールバックするため、返す答えは常に厳密である。

## `SubmodularMinimizationResult<Value>`

- `elements`, `value`: 厳密な最小化集合（要素番号の昇順）とそのオラクル値。
- `minimum_norm_point`: 基多面体上の候補点。
- `iterations`: Wolfe 法の反復回数。
- `dual_gap`: 停止時の双対ギャップ。
- `minimum_norm_converged`: 数値反復がギャップ条件で収束したかどうか。
- `exact_certificate_used`: 基多面体による下界の証明で全列挙を省略したかどうか。

## `submodular_function_minimization(ground_set_size, oracle, maximum_iterations = 10000, tolerance = 1e-12)`

`oracle(set)` は長さ $n$ の 0/1 の `std::vector<unsigned char>` を受け取り、その集合の関数値を返す。戻り値の型が `Value` になる。$n = 0$ なら空集合を返す。

## 時間計算量

アクティブな基の個数を $a$、実行した Wolfe 法の反復回数を $I$、オラクル 1 回の時間を $T$ とする。各反復で貪欲法による端点の計算（$O(nT + n \log n)$）と、高々 $a$ 回の内部反復（それぞれ Gram 行列の構築 $O(a^2 n)$ と連立一次方程式の求解 $O(a^3)$）を行うため、最小ノルム点の計算は $O(I(nT+n\log n+a^4+a^3n))$ 時間である。

候補点の座標順に作った 1 本の貪欲な基について、限界差分が負のものがすべて非負のものより前に並ぶ場合は、その符号と tight な接頭辞から厳密な最適性を証明する。整数値のオラクルではさらに、アクティブな基の係数を分母 $2^{48}$ の非負有理数に丸め、和をちょうど $1$ に保った基多面体上の点について、負の座標の和が候補値と一致するかを `__int128` で検査する。これらの証明に成功した場合の追加時間は $O(nT+an)$ で、指数時間の部分はない。

証明に失敗した場合だけ $2^n$ 個の部分集合を照合する。この場合を含む最悪時間計算量は $O(I(nT+n\log n+a^4+a^3n)+2^n(n+T))$ である。

## 空間計算量

$O(an+a^2+n)$。

## 注意点

- オラクルの値は有限の数値に変換できなければならず、関数は劣モジュラでなければならない。非有限の値を返した場合は `std::invalid_argument` を送出する。
- 最小ノルム点の計算は `long double` で行うが、`elements` と `value` の厳密性には影響しない。
- 整数値の証明は 64 ビット以下の組み込み整数型を対象とし、中間積には `__int128` を使う。
- 停止条件（`maximum_iterations == 0`、正でない・非有限の `tolerance`）が不正なら `std::invalid_argument`、アクティブな基が数値的に特異なら `std::runtime_error` を送出する。
- 最適性の証明が成立しない入力では全部分集合を評価するため、$n$ が大きいときに常に高速に終了する保証はない。
