---
title: Xorshift State Cracker (Xorshift状態復元)
documentation_of: ../src/algorithm/random/crack/xorshift_cracker.hpp
---

Xorshiftの完全な生出力から、観測直前の状態を逆算して次の出力を予測する。
完全な生出力を保存できない場合でも、<code>XorShift32</code>、
<code>XorShift64</code>、<code>XorShift128</code>はそれぞれ状態bit数と
同数の連続出力LSB（出力を2で割った余り）から観測直前の状態を復元できる。

## recover_xorshift32_seed

~~~cpp
std::uint32_t recover_xorshift32_seed(std::uint32_t first_output)
~~~

<code>XorShift32</code> の1出力から、その出力を生成する直前の状態を返す。
seed設定後の最初の出力を渡した場合、その戻り値がseedになる。

## XorShift32のLSB状態復元

~~~cpp
inline constexpr std::size_t xorshift32_lsb_observation_count = 32;
std::size_t xorshift32_lsb_observation_rank();
std::uint32_t recover_xorshift32_seed_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb
);
~~~

連続32出力のLSBを古い順に渡し、最初の出力を生成する直前の32bit状態を
返す。観測行列のrankは32である。要素数が32でないか、0と1以外の値を
含む場合は <code>std::invalid_argument</code> を送出する。全要素0は
線形系の状態0へ復元されるが、生成器本体は0 seedを受理しない。

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

## XorShift128のLSB状態復元

~~~cpp
inline constexpr std::size_t xorshift128_lsb_observation_count = 128;
std::size_t xorshift128_lsb_observation_rank();
XorShift128::state_type recover_xorshift128_state_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb
);
~~~

連続128出力のLSBを古い順に渡し、最初の出力を生成する直前の4語状態を
返す。観測行列のrankは128である。要素数が128でないか、0と1以外の値を
含む場合は <code>std::invalid_argument</code> を送出する。全要素0は
線形系の全語0状態へ復元されるが、生成器本体は全語0をseedとして受理しない。

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

- 各 <code>*_lsb_observation_count</code> の参照：$O(1)$ 時間。
- <code>XorShift64</code> のrank参照：$O(1)$ 時間。
  <code>recover_xorshift64_seed_from_lsb()</code> は64要素の検査と
  64本の64bit内積により $O(64)=O(1)$ 時間、$O(1)$ 追加空間。
- <code>XorShift32</code> / <code>XorShift128</code> は状態bit数を
  $N$ として、rankまたは復元の初回呼出し時に
  $O(N^2+N^3/64)$ 時間・$O(N^2/64)$ 保存領域で観測逆行列を構築する。
  同じprocess内の以後のrank参照は $O(1)$、復元は
  $O(N^2/64)$ 時間・$O(N/64)$ 追加領域。

## 注意点

shiftパラメータが既知で、加工・切り詰めのない連続した生出力が必要。
観測前の呼出回数が不明なら復元できるのは観測直前の状態であり、最初に設定した
seedとは限らない。このheaderの完全出力逆算APIはXorshift+、
Xorshift*、xoroshiro、xoshiroを対象にしない。これらのうちLSBが線形観測に
なる生成器には <code>linear_prng_lsb_cracker.hpp</code> と
<code>large_state_linear_prng_lsb_cracker.hpp</code> を使う。
LSB復元では間の出力を1個でも落とすと別の状態を復元するため、
必ず各APIの観測数定数が示す個数を連続させ、古い順で渡す。満rankなので
0/1からなる正しい長さの列には常に一意な線形状態が対応し、その列が実際に
許可された非零seedから始まったかまでは判定しない。
