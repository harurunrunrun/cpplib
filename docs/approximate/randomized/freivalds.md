---
title: Freivalds' Algorithm (Freivalds のアルゴリズム)
documentation_of: ../../../src/approximate/randomized/freivalds.hpp
---

行列積 `first * second == product` が成り立つかを乱択で検査する。`vector<vector<T>>` 版と固定長 `array` 版のオーバーロードがある。

## API

```cpp
bool freivalds_verify(first, second, product, rounds = 20, seed = 0);
```

各ラウンドで成分が 0 または 1 のベクトル $r$ をシード `seed` の乱数で作り、`first * (second * r)` と `product * r` を比較する。

行列の形を $R \times I$, $I \times C$, $R \times C$、ラウンド数を $T$ とすると、時間計算量は $O(T(RI + IC + RC))$、追加空間計算量は $O(R + I + C)$。

正しい積に対しては必ず `true` を返す。誤った積を `true` と判定する確率は高々 $2^{-T}$ である。

## 注意点

- `rounds == 0`、矩形でない行列、形が合わない行列に対しては `std::invalid_argument` を送出する。
- 演算型 `T` は 0, 1、加算、乗算、等値比較が使え、途中の値が表現範囲に収まらなければならない。
- `seed` のデフォルトは 0 で固定なので、敵対的な入力に対しては呼び出し側で乱数のシードを与えること。
- 確率的保証は各ラウンドの乱数が独立である場合に成り立つ。
