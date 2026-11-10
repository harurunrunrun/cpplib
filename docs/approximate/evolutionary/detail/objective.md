---
title: Evolutionary Objective Helper (進化計算の目的関数補助)
documentation_of: ../../../../src/approximate/evolutionary/detail/objective.hpp
---

進化計算の目的関数を、検査付きで評価する内部 API である。

## API

### `evaluate_objective(evaluate, individual)`

`evaluate(individual)` を 1 回呼び、その値を返す。戻り値が浮動小数点型なら有限でなければならない。目的関数の時間・追加空間をそれぞれ $T_E$, $S_E$ として、時間計算量は $O(T_E)$、追加空間計算量は $O(S_E)$ である。

## 注意点

- `approximate::evolutionary::detail` の内部 API である。
- 浮動小数点の評価値が非有限なら `std::domain_error` を送出する。目的関数が送出した例外はそのまま伝播する。
