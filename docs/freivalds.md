---
title: Freivalds' Algorithm (Freivaldsのアルゴリズム)
documentation_of: ../src/approximate/randomized/freivalds.hpp
---

`first * second == product` を乱択で検査する。`vector<vector<T>>` と固定長 `array`
の overload がある。

- `freivalds_verify(first, second, product, rounds, seed)`: 各 round で 0/1 vector `r` を作り、
  `first * (second * r)` と `product * r` を比較する。
  行列 shape を $R\times I$, $I\times C$ とすると
  $O(rounds(RI+IC+RC))$、追加領域 $O(R+I+C)$。

正しい積は必ず `true`。異なる積を `true` とする確率は高々 $2^{-rounds}$。
`rounds == 0` や非矩形・非互換 shape は `std::invalid_argument`。
演算型 `T` は 0, 1、加算、乗算、等値比較を使え、途中値が表現範囲に収まる必要がある。
