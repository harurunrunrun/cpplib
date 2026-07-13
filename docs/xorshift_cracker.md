---
title: Xorshift State Cracker (Xorshift状態復元)
documentation_of: ../src/algorithm/random/xorshift_cracker.hpp
---

Xorshiftの完全な生出力から、観測直前の状態を逆算して次の出力を予測する。
生出力を保存できない場合でも、<code>XorShift64</code>は連続64出力の
LSB（出力を2で割った余り）から復元できる。

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

## xorshift64_lsb_observation_count

~~~cpp
inline constexpr std::size_t xorshift64_lsb_observation_count = 64;
~~~

LSBによる復元に必要な連続出力数。

## xorshift64_lsb_observation_rank

~~~cpp
constexpr std::size_t xorshift64_lsb_observation_rank() noexcept
~~~

64個の観測式が作る $GF(2)$ 上の行列のrankを返す。
現在の $(13,7,17)$ 遷移では64であり、ヘッダ内の
<code>static_assert</code>で満rankであることを検査する。

## recover_xorshift64_seed_from_lsb

~~~cpp
constexpr std::uint64_t recover_xorshift64_seed_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb
)
~~~

<code>XorShift64</code>の連続する64出力を古い順に並べ、それぞれの
LSBを0または1で渡す。$GF(2)$ 上の逆観測行列を使い、
1個目の出力を生成する直前の64bit状態を返す。seed設定直後から
観測した場合は、そのseed自体が戻り値になる。

観測数が64でない、または0と1以外の値が含まれる場合は
<code>std::invalid_argument</code>。全要素0は線形系の解0を返すが、
<code>XorShift64</code>本体は0 seedを受理しない。

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

- <code>xorshift64_lsb_observation_count</code> の参照：$O(1)$ 時間。
- <code>xorshift64_lsb_observation_rank()</code>：$O(1)$ 時間。
- <code>recover_xorshift64_seed_from_lsb()</code>：64要素の検査と
  64本の64bit内積により $O(64)=O(1)$ 時間、$O(1)$ 追加空間。
  一般の $w$ bit語として表すと $O(w^2/\text{word size})$ 時間。

## 注意点

shiftパラメータが既知で、加工・切り詰めのない連続した生出力が必要。
観測前の呼出回数が不明なら復元できるのは観測直前の状態であり、最初に設定した
seedとは限らない。Xorshift+、Xorshift*、xoroshiroには適用できない。
LSB復元では間の出力を1個でも落とすと別の状態を復元するため、
必ず連続した64個を古い順で渡す。
