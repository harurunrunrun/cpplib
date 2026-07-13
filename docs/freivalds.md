---
title: Freivalds' Algorithm (Freivaldsのアルゴリズム)
documentation_of: ../src/approximate/randomized/freivalds.hpp
---

`first * second == product` を乱択で検査する。`vector<vector<T>>` と固定長 `array`
の overload がある。

- `freivalds_verify(first, second, product, rounds, seed)`: 各 round で 0/1 vector `r` を作り、
  `first * (second * r)` と `product * r` を比較する。
  行列 shape を $R\times I$, $I\times C$ とすると
  時間計算量は $O(rounds(RI+IC+RC))$、追加空間計算量は $O(R+I+C)$。

正しい積は必ず `true`。異なる積を `true` とする確率は高々 $2^{-rounds}$。
`rounds == 0` や非矩形・非互換 shape は `std::invalid_argument`。
演算型 `T` は 0, 1、加算、乗算、等値比較を使え、途中値が表現範囲に収まる必要がある。

## 注意点

確率的保証は各APIで示した独立性と入力条件の下で成り立つ。同じ結果の再現には同じ乱数器状態・入力・標準ライブラリ実装が必要で、中間演算は使用型の表現範囲内でなければならない。
