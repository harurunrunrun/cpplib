---
title: Powell's Direction Set Method (Powell方向集合法)
documentation_of: ../../../src/approximate/optimization/powell.hpp
---

## `powell`

```cpp
auto result = approximate::optimization::powell(
    initial, objective, max_iterations, max_evaluations,
    initial_step = 1, line_reduction = 0.5, line_tolerance = 1e-6,
    tolerance = 1e-8, lower = {}, upper = {}
);
```

単位座標方向から始める、勾配を使わない Powell の方向集合法である。各方向では正方向、負方向の順に候補を比較する決定的なパターン直線探索を行い、改善がなければステップ幅を `line_reduction` 倍する。1 回の走査（スイープ）全体の変位を正規化し、最大の減少を得た方向と入れ替える。目的値が等しい候補では、先に調べた正方向を保持する。

初期点の目的関数値と受理した候補の値を保持し、同じ点を不必要に再評価しない。箱への射影で現在点と同じになった候補も評価しない。返り値は `OptimizationResult<Real>` で、`gradient_evaluations == 0` である。`evaluations <= max_evaluations` であり、評価回数の上限に達した場合は、その時点までの最良点を未収束として返す。スイープでの変位がなくなるか、点と値の変化が `tolerance` 以下になれば収束とみなす。

## 計算量

次元を $D$、スイープ数を $I$、目的関数 1 回のコストを $F$、構築した直線探索の候補の総数を $Q$、実際の目的関数の評価の総数を $L$ とする。時間計算量は $O(D^2+ID^2+QD+LF)$、追加空間計算量は方向集合を含めて $O(D^2)$ である。結果の各メンバの参照は $O(1)$、結果の点の列挙は $O(D)$ である。

## 注意点

- 初期点は空でない有限値でなければならない（0 次元は `std::invalid_argument`）
- `max_evaluations`, `initial_step`, `line_tolerance` は正、ステップ幅は有限かつ `line_tolerance <= initial_step`、`line_reduction` は $(0,1)$、`tolerance` は 0 以上の有限値でなければならない。箱制約は次元が一致し、有限で、`lower[i] <= upper[i]` を満たし、初期点を含まなければならない。違反は `std::invalid_argument` を送出する
- 目的関数の NaN・無限大には `std::domain_error`、候補・変位のオーバーフローには `std::overflow_error` を送出する。浮動小数点の丸めにより、`tolerance` より大きいステップ幅をそれ以上縮小できない場合は、直線探索の失敗として `std::runtime_error` を送出する
- 非凸関数に対する大域最適解や、反復上限内での収束は保証しない
