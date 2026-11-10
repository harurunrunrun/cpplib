---
title: Power Tower Last Digits (累乗塔の末尾桁) [POWTOW]
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/power_tower_last_digits.hpp
---

同じ底を右結合で積み重ねた累乗塔 $b \uparrow\uparrow h = b^{b^{\cdot^{\cdot^{b}}}}$（$b$ が $h$ 個）について、10 進の末尾 $D$ 桁と、真の値が $10^D$ 以上かどうかを求める。

## `TetrationLastDigits`

```cpp
struct TetrationLastDigits {
    std::uint64_t value;
    unsigned digit_count;
    bool truncated;
    std::string to_string() const;
};
```

`value` は末尾 `digit_count` 桁の値、`truncated` は真の値が $10^D$（$D$ は `digit_count`）以上かどうかを表す。`to_string()` は、`truncated` なら `...` に続けて $D$ 桁に 0 埋めした末尾桁を、そうでなければ値をそのまま文字列にして返す。

## `tetration_last_digits`

```cpp
math::TetrationLastDigits math::tetration_last_digits(
    std::uint64_t base,
    std::uint64_t height,
    unsigned digit_count
);
```

$\mathrm{base} \uparrow\uparrow \mathrm{height}$ の末尾 `digit_count` 桁と、その桁数に収まるかどうかを `TetrationLastDigits` で返す。

- `base`: 累乗塔で繰り返す非負整数の底。
- `height`: 累乗塔の高さ。高さ $0$ の値は $1$ とする。
- `digit_count`: 求める 10 進の末尾桁数 $D$。$1 \le D \le 19$。

オイラーの定理の一般化（指数が $\varphi(m)$ 以上なら $\varphi(m)$ を法として還元した値に $\varphi(m)$ を足してよい）を用いて、法を $\varphi$ で順に小さくしながら再帰的に計算する。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `TetrationLastDigits::to_string` | $O(D)$ | $O(D)$ |
| `tetration_last_digits` | $O(D^2)$ | $O(D)$ |

## 注意点

- $0 \uparrow\uparrow 0 = 1$ と定義する。
- 底が $0$ の場合、高さが偶数なら $1$、奇数なら $0$ になる。
- 桁数が範囲外なら `std::invalid_argument` を送出する。
