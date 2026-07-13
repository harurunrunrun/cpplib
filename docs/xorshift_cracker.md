---
title: Xorshift State Cracker (Xorshift状態復元)
documentation_of: ../src/algorithm/random/xorshift_cracker.hpp
---

Xorshiftの完全な生出力から、観測直前の状態を逆算して次の出力を予測する。

## recover_xorshift32_seed

~~~cpp
std::uint32_t recover_xorshift32_seed(std::uint32_t first_output)
~~~

<code>XorShift32</code> の1出力から、その出力を生成する直前の状態を返す。
seed設定後の最初の出力を渡した場合、その戻り値がseedになる。

## recover_xorshift64_seed

~~~cpp
std::uint64_t recover_xorshift64_seed(std::uint64_t first_output)
~~~

<code>XorShift64</code> に対する同じ逆変換。

## XorShift128Cracker

~~~cpp
explicit XorShift128Cracker(
    const std::array<std::uint32_t, 4>& four_consecutive_outputs
)
~~~

連続4出力を受け取り、その直後のXorshift128状態を作る。全出力0なら
<code>std::invalid_argument</code>。

- <code>result_type</code>：<code>XorShift128::result_type</code>。
- <code>state_type</code>：<code>XorShift128::state_type</code>。
- コンストラクタ：連続4出力から観測直前の状態と予測器を構築する。
- <code>initial_state()</code>：最初の観測値を生成する直前の4語を返す。
- <code>current_state()</code>：予測器の現在状態を返す。
- <code>next()</code> / <code>operator()()</code>：次の出力を返して予測器を進める。

## 時間計算量

32bit・64bit seed復元、<code>XorShift128Cracker</code> の構築、
<code>initial_state</code>、<code>current_state</code>、<code>next</code>、
<code>operator()</code>、型別名の参照は
すべて $O(1)$ 時間・$O(1)$ 追加空間。

## 注意点

shiftパラメータが既知で、加工・切り詰めのない連続した生出力が必要。
観測前の呼出回数が不明なら復元できるのは観測直前の状態であり、最初に設定した
seedとは限らない。Xorshift+、Xorshift*、xoroshiroには適用できない。
