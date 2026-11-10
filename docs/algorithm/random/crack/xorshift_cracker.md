---
title: Xorshift State Cracker (Xorshift状態復元)
documentation_of: ../../../../src/algorithm/random/crack/xorshift_cracker.hpp
---

Xorshift の完全な生出力から、観測直前の状態を逆算して次の出力を予測する。完全な生出力を保存できない場合でも、`XorShift32`、`XorShift64`、`XorShift128` はそれぞれ状態ビット数と同数の連続出力 LSB（出力を 2 で割った余り）から観測直前の状態を復元できる。

## recover_xorshift32_seed

~~~cpp
constexpr std::uint32_t recover_xorshift32_seed(std::uint32_t first_output)
~~~

`XorShift32` の 1 出力から、その出力を生成する直前の状態を返す。シード設定後の最初の出力を渡した場合、その戻り値がシードになる。

## XorShift32 の LSB 状態復元

~~~cpp
inline constexpr std::size_t xorshift32_lsb_observation_count = 32;
std::size_t xorshift32_lsb_observation_rank();
std::uint32_t recover_xorshift32_seed_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb
);
~~~

連続 32 出力の LSB を古い順に渡し、最初の出力を生成する直前の 32 bit 状態を返す。観測行列のランクは 32 である。要素数が 32 でないか、0 と 1 以外の値を含む場合は `std::invalid_argument` を送出する。全要素 0 は線形系の状態 0 へ復元されるが、生成器本体は 0 シードを受理しない。

## recover_xorshift64_seed

~~~cpp
constexpr std::uint64_t recover_xorshift64_seed(std::uint64_t first_output)
~~~

`XorShift64` に対する同じ逆変換を行う。

## xorshift64_lsb_observation_count

~~~cpp
inline constexpr std::size_t xorshift64_lsb_observation_count = 64;
~~~

LSB による復元に必要な連続出力数である。

## xorshift64_lsb_observation_rank

~~~cpp
constexpr std::size_t xorshift64_lsb_observation_rank() noexcept
~~~

64 個の観測式が作る $GF(2)$ 上の行列のランクを返す。現在の $(13,7,17)$ 遷移では 64 であり、ヘッダ内の `static_assert` でフルランクであることを検査する。

## recover_xorshift64_seed_from_lsb

~~~cpp
constexpr std::uint64_t recover_xorshift64_seed_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb
)
~~~

`XorShift64` の連続する 64 出力を古い順に並べ、それぞれの LSB を 0 または 1 で渡す。$GF(2)$ 上の逆観測行列を使い、1 個目の出力を生成する直前の 64 bit 状態を返す。シード設定直後から観測した場合は、そのシード自体が戻り値になる。

観測数が 64 でない、または 0 と 1 以外の値が含まれる場合は `std::invalid_argument` を送出する。全要素 0 は線形系の解 0 を返すが、`XorShift64` 本体は 0 シードを受理しない。

## XorShift128 の LSB 状態復元

~~~cpp
inline constexpr std::size_t xorshift128_lsb_observation_count = 128;
std::size_t xorshift128_lsb_observation_rank();
XorShift128::state_type recover_xorshift128_state_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb
);
~~~

連続 128 出力の LSB を古い順に渡し、最初の出力を生成する直前の 4 語状態を返す。観測行列のランクは 128 である。要素数が 128 でないか、0 と 1 以外の値を含む場合は `std::invalid_argument` を送出する。全要素 0 は線形系の全語 0 状態へ復元されるが、生成器本体は全語 0 をシードとして受理しない。

## XorShift128Cracker

~~~cpp
explicit XorShift128Cracker(
    const std::array<std::uint32_t, 4>& four_consecutive_outputs
)
~~~

連続 4 出力を受け取り、その直後の Xorshift128 状態を作る。全出力が 0 なら `std::invalid_argument` を送出する。

- `result_type`: `XorShift128::result_type`。
- `state_type`: `XorShift128::state_type`。
- コンストラクタ: 連続 4 出力から観測直前の状態と予測器を構築する。
- `initial_state()`: 最初の観測値を生成する直前の 4 語を返す。
- `current_state()`: 予測器の現在状態を返す。
- `next()` / `operator()()`: 次の出力を返して予測器を進める。

## 時間計算量

32 bit・64 bit のシード復元、`XorShift128Cracker` の構築、`initial_state`、`current_state`、`next`、`operator()`、型別名の参照はすべて $O(1)$ 時間・$O(1)$ 追加空間。

- 各 `*_lsb_observation_count` の参照: $O(1)$ 時間。
- `XorShift64` のランク参照: $O(1)$ 時間。`recover_xorshift64_seed_from_lsb()` は 64 要素の検査と 64 本の 64 bit 内積により $O(64)=O(1)$ 時間、$O(1)$ 追加空間。
- `XorShift32` / `XorShift128` は状態ビット数を $N$ として、ランクまたは復元の初回呼出し時に $O(N^2+N^3/64)$ 時間・$O(N^2/64)$ 保存領域で観測逆行列を構築する。同じプロセス内の以後のランク参照は $O(1)$、復元は $O(N^2/64)$ 時間・$O(N/64)$ 追加領域。

## 注意点

- シフトのパラメータが既知で、加工・切り詰めのない連続した生出力が必要である。
- 観測前の呼出し回数が不明な場合に復元できるのは観測直前の状態であり、最初に設定したシードとは限らない。
- このヘッダの完全出力逆算 API は Xorshift+、Xorshift*、xoroshiro、xoshiro を対象にしない。これらのうち LSB が線形観測になる生成器には `linear_prng_lsb_cracker.hpp` と `large_state_linear_prng_lsb_cracker.hpp` を使う。
- LSB 復元では間の出力を 1 個でも落とすと別の状態を復元するため、各 API の観測数定数が示す個数の連続出力を必ず古い順で渡す。
- 観測行列はフルランクなので、0/1 からなる正しい長さの列には常に一意な線形状態が対応する。その列が実際に許可された非零シードから始まったかまでは判定しない。
