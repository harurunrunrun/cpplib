---
title: Linear PRNG LSB Cracker (線形PRNGのLSB状態復元)
documentation_of: ../src/algorithm/random/crack/linear_prng_lsb_cracker.hpp
---

Xorshift128+、Xoroshiro128+、Xoshiro256+、WELL512aについて、
完全な出力値ではなく連続出力のLSB（出力を2で割った余り）だけから
内部状態を復元する。各観測行列は初回利用時に一度だけ構築して再利用する。

## Xorshift128+ API

~~~cpp
struct Xorshift128PlusState {
    std::uint64_t first;
    std::uint64_t second;
};
std::uint64_t xorshift128plus_next(Xorshift128PlusState& state) noexcept;
inline constexpr std::size_t xorshift128plus_lsb_observation_count = 128;
std::size_t xorshift128plus_lsb_observation_rank();
Xorshift128PlusState recover_xorshift128plus_state_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb
);
~~~

$(23,18,5)$遷移のXorshift128+を1回進めるAPI、必要観測数、観測行列の
rank、状態復元APIを提供する。連続128出力のLSBを古い順に渡すと、
最初の観測直前の128bit状態を返す。観測行列のrankは128である。

## `Xoroshiro128PlusState`

~~~cpp
struct Xoroshiro128PlusState {
    std::uint64_t first;
    std::uint64_t second;
};
~~~

Xoroshiro128+の128bit内部状態。

## `xoroshiro128plus_next()`

~~~cpp
std::uint64_t xoroshiro128plus_next(Xoroshiro128PlusState& state) noexcept;
~~~

Xoroshiro128+ 1.0の$(55,14,36)$遷移で次の値を返し、状態を進める。

## Xoroshiro128+の観測情報

~~~cpp
inline constexpr std::size_t xoroshiro128plus_lsb_observation_count = 128;
std::size_t xoroshiro128plus_lsb_observation_rank();
~~~

必要な連続LSB数と観測行列のrankを返す。rankは128。

## `recover_xoroshiro128plus_state_from_lsb()`

~~~cpp
Xoroshiro128PlusState recover_xoroshiro128plus_state_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb
);
~~~

連続128出力のLSBを古い順に渡し、最初の観測直前の状態を返す。

## Xoshiro256+ API

~~~cpp
struct Xoshiro256PlusState {
    std::array<std::uint64_t, 4> words;
};
std::uint64_t xoshiro256plus_next(Xoshiro256PlusState& state) noexcept;
inline constexpr std::size_t xoshiro256plus_lsb_observation_count = 256;
std::size_t xoshiro256plus_lsb_observation_rank();
Xoshiro256PlusState recover_xoshiro256plus_state_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb
);
~~~

Xoshiro256+ 1.0を1回進めるAPI、必要観測数、観測行列のrank、
状態復元APIを提供する。連続256出力のLSBを古い順に渡すと、
最初の観測直前の256bit状態を返す。観測行列のrankは256である。

## `Well512aState`

~~~cpp
struct Well512aState {
    std::array<std::uint32_t, 16> words;
    std::size_t index;
};
~~~

WELL512aの512bit内部状態と現在位置。復元APIは呼び出し側が指定した
既知の`index`を保った状態を返す。

## `well512a_next()`

~~~cpp
std::uint32_t well512a_next(Well512aState& state) noexcept;
~~~

WELL512aの次の値を返し、状態と現在位置を進める。

## WELL512aの観測情報

~~~cpp
inline constexpr std::size_t well512a_lsb_observation_count = 512;
std::size_t well512a_lsb_observation_rank();
~~~

必要な連続LSB数と観測行列のrankを返す。rankは512。

## `recover_well512a_state_from_lsb()`

~~~cpp
Well512aState recover_well512a_state_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb,
    std::size_t initial_index = 0
);
~~~

既知の`initial_index`から得た連続512出力のLSBを古い順に渡し、
最初の観測直前の状態を返す。`initial_index`は`[0, 16)`でなければならず、
範囲外なら`std::invalid_argument`を送出する。

## 時間計算量

- 各`*_next()`と観測数定数の参照：$O(1)$時間、$O(1)$追加空間。
- 各`*_lsb_observation_rank()`：初回だけ観測行列の構築に、状態bit数を
  $N$として$O(N^3/64)$時間と$O(N^2/64)$空間。以後は$O(1)$時間。
- 各`recover_*_state_from_lsb()`：初回だけ上記の構築を行う。
  以後は$O(N^2/64)$時間、$O(N/64)$追加空間。
- 各状態の比較と変換：状態bit数を$N$として$O(N/64)$時間。

## 注意点

入力は0または1だけで、指定個数の連続したLSBでなければならない。
各`+`出力のLSBには下位bitからのcarryが入らないためGF(2)上で線形だが、
加算結果の上位bit列にはこの復元法をそのまま適用できない。WELL512aの
`initial_index`は観測から復元する値ではなく、呼び出し側が知っている
巡回配列の位相である。いずれも暗号学的安全性を持つ乱数器ではない。
