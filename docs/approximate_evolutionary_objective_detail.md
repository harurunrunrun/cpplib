---
title: Evolutionary Objective Helper (進化計算の目的関数補助)
documentation_of: ../src/approximate/evolutionary/detail/objective.hpp
---

# Evolutionary Objective Helper (進化計算の目的関数補助)

進化計算の目的関数を検査付きで評価する内部API。

## API

### `evaluate_objective(evaluate, individual)`

`evaluate(individual)` を一度呼び、その値を返す。浮動小数点型の返値は有限でなければならない。目的関数の時間・追加空間計算量をそれぞれ $T_E,S_E$ として、時間計算量は $O(T_E)$、追加空間計算量は $O(S_E)$。

## 注意点

`approximate::evolutionary::detail` の内部APIである。浮動小数点の非有限scoreでは `std::domain_error` を送出する。目的関数が送出する例外はそのまま伝播する。
