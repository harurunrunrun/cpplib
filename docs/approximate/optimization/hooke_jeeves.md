---
title: Hooke–Jeeves Method (Hooke–Jeeves法)
documentation_of: ../../../src/approximate/optimization/hooke_jeeves.hpp
---

## `hooke_jeeves`

```cpp
auto result = approximate::optimization::hooke_jeeves(
    initial, objective, max_iterations, max_evaluations,
    initial_step = 1, reduction_factor = 0.5, pattern_factor = 1,
    tolerance = 1e-8, lower = {}, upper = {}
);
```

各座標について正方向、負方向の順に探査移動（exploratory move）を行う。改善する方向が見つかった座標では反対側を評価せず、その時点の改善点から次の座標を調べる。探査移動全体で改善したら、パターン移動の点 `explored + pattern_factor * (explored - base)` も 1 回調べ、さらに改善する場合だけ採用する。改善がなければステップ幅を `reduction_factor` 倍し、`step <= tolerance` で収束とみなす。乱数は使わず、目的値が等しい場合は先に調べた候補を保持する。

目的関数の値を点とともにキャッシュし、射影後に現在点と同じになった候補や、正方向で改善済みの場合の負方向は評価しない。返り値は `OptimizationResult<Real>` で、`gradient_evaluations == 0`、`evaluations <= max_evaluations` である。評価回数・反復回数の上限に達した場合は、その時点の最良点を未収束として返す。

## 計算量

次元を $D$、反復回数を $I$、目的関数 1 回のコストを $F$ とする。候補は 1 反復で最大 $2D+1$ 個で、目的関数の呼び出しは最大 $1+I(2D+1)$ 回である。現在の実装は各座標の候補点をコピーするため、時間計算量は $O(F+I(D^2+DF))$、追加空間計算量は $O(D)$ である。結果のメンバの参照は $O(1)$、結果の点の列挙は $O(D)$ である。

## 注意点

- 初期点は空でない有限値でなければならない（0 次元は `std::invalid_argument`）
- `max_evaluations`, `initial_step`, `pattern_factor` は正、`reduction_factor` は $(0,1)$、`tolerance` は 0 以上で、実数のパラメータはすべて有限でなければならない。箱制約は次元が一致し、有限で、`lower[i] <= upper[i]` を満たし、初期点を含まなければならない。違反は `std::invalid_argument` を送出する
- 目的関数の非有限値には `std::domain_error`、候補の計算のオーバーフローには `std::overflow_error`、丸めによりステップ幅を縮小できない場合は `std::runtime_error` を送出する
- 非凸関数に対する大域最適解や、反復上限内での収束は保証しない
