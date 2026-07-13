---
title: BFGS / L-BFGS (準ニュートン法)
documentation_of: ../src/approximate/optimization/quasi_newton.hpp
---

滑らかな目的関数を勾配とArmijo backtracking line searchで最小化する準Newton法。
返却値は [`OptimizationResult<Real>`](optimization_result.md) である。

## `bfgs`

```cpp
auto result = approximate::optimization::bfgs(
    initial, objective, gradient, max_iterations, max_evaluations,
    initial_step, tolerance, armijo, line_reduction, minimum_step,
    lower, upper
);
```

逆Hessian近似を単位行列から開始し、曲率条件を満たす更新対にBFGS更新を行う。
`gradient`は点を受け取り同じ次元の`vector<Real>`を返す。勾配の無限大normが
`tolerance`以下なら収束する。探索方向が降下方向でなくなった場合は逆Hessian近似を
単位行列へ戻す。

各反復の目的関数値はline searchが返した値を再利用する。初期値を1回評価した後、
`evaluations`はArmijo候補を実際に評価した回数だけ増え、`max_evaluations`を超えない。
`gradient_evaluations`は初期勾配と受理された更新後の勾配の回数である。

次元を $D$、受理更新数を $I$、目的関数1回を $F$、勾配1回を $G$、line searchの
目的関数評価総数を $L$ とする。時間は
$O(D^2+I(D^2+G)+L(D+F))$、追加領域は $O(D^2)$。
点・目的関数・勾配・結果memberの参照はそれぞれ $O(1)$、結果点の列挙は $O(D)$。

## `lbfgs`

```cpp
auto result = approximate::optimization::lbfgs(
    initial, objective, gradient, max_iterations, max_evaluations,
    history_size, initial_step, tolerance, armijo,
    line_reduction, minimum_step, lower, upper
);
```

直近`history_size`個の曲率対だけを保持し、two-loop recursionで探索方向を求める
L-BFGS。十分な正曲率を持たない対は保存せず、降下方向でなくなれば履歴を破棄する。
同じ入力の目的関数・勾配に対して乱数を使わず決定的に動作する。

履歴上限を $M$ とすると、時間は
$O(D+I(MD+G)+L(D+F))$、追加領域は $O(MD)$。
目的関数・勾配の計数規則は`bfgs`と同じである。

## parameter・例外・停止

`Real`は浮動小数点型で、初期点は空でない有限値でなければならない。したがって0次元は
`std::invalid_argument`。`max_evaluations`と`history_size`は正、`initial_step`と
`minimum_step`は正の有限値かつ`minimum_step <= initial_step`、`armijo`と
`line_reduction`は`(0,1)`、`tolerance`は0以上の有限値が必要である。
`max_iterations == 0`は初期目的関数だけを評価して未収束で返す。

箱制約を使う場合は両端の次元が一致し、有限かつ`lower[i] <= upper[i]`で、初期点が箱内に
なければならない。候補は箱へ射影する。準Newton方向を射影すると降下移動がなくなる場合は
Hessian近似または履歴をresetして射影最急降下方向を再試行し、その方向にも実行可能な
降下移動がない場合だけ収束とする。
評価上限到達・反復上限到達は`converged == false`で通常終了する。実際に評価した候補が
`minimum_step`までArmijo条件を満たさない場合は`std::runtime_error`。

不正parameter・勾配次元には`std::invalid_argument`、目的関数または勾配のNaN/無限大には
`std::domain_error`、点・内積・行列更新のoverflowには`std::overflow_error`を送出する。
