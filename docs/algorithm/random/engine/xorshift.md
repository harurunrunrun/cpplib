---
title: Xorshift Engines (Xorshift乱数生成器)
documentation_of: ../../../../src/algorithm/random/engine/xorshift.hpp
---

XOR とビット shift で状態を更新する固定状態乱数生成器。

## XorShift32

~~~cpp
XorShift32()
explicit XorShift32(std::uint32_t seed)
~~~

Marsaglia の $(13,17,5)$ 遷移を使う 32bit 版。

- `result_type`：`std::uint32_t`。
- `default_seed`：既定シード `2463534242`。
- 各コンストラクタ：既定シードまたは指定したシードで構築する。
- `seed(value)`：状態を `value` へ戻す。引数を省略した場合は既定シードを使う。0 なら `std::invalid_argument`。
- `operator()()`：状態を 3 回の XOR shift で更新し、更新後の値を返す。
- `state()`：現在の 32bit 状態を返す。
- `min()` / `max()`：生成範囲の下限 1 と上限 `UINT32_MAX` を返す。

## XorShift64

~~~cpp
XorShift64()
explicit XorShift64(std::uint64_t seed)
~~~

$(13,7,17)$ 遷移を使う 64bit 版。`result_type` は `std::uint64_t`、`default_seed` は `88172645463325252`。コンストラクタ、`seed`、`operator()`、`state`、`min`、`max` の意味は `XorShift32` と同じ。

## XorShift128

~~~cpp
XorShift128()
explicit XorShift128(const std::array<std::uint32_t, 4>& seed)
XorShift128(std::uint32_t x, std::uint32_t y,
            std::uint32_t z, std::uint32_t w)
~~~

4 語の Marsaglia Xorshift128。

- `result_type`：`std::uint32_t`。
- `state_type`：`std::array<result_type, 4>`。
- `default_seed`：4 語の既定状態。
- 各コンストラクタ：既定状態、配列、または 4 個の整数から構築する。
- `seed(state)`：4 語を置換する。引数を省略した場合は既定状態へ戻す。

`seed(state)` は全語 0 なら `std::invalid_argument`。`operator()()` は次の 32bit 値、`state()` は現在の 4 語を返す。`min()` は 0、`max()` は `UINT32_MAX`。

## 時間計算量

全コンストラクタ、`seed`、`operator()`、`state`、`min`、`max` は時間・追加空間計算量ともに $O(1)$。型別名と静的定数の参照も $O(1)$。

## 注意点

0 状態は Xorshift の吸収状態なので受理しない。完全周期はここで固定したパラメータと非零状態についての性質であり、任意の shift 列へ一般化した型ではない。暗号用途には使用しない。
