---
title: Linear PRNG LSB Cracker (線形PRNGのLSB状態復元)
documentation_of: ../src/algorithm/random/crack/linear_prng_lsb_cracker.hpp
---

Xoroshiro128+とWELL512aについて、完全な出力値ではなく連続出力の
LSB（出力を2で割った余り）だけから内部状態を復元する。
どちらも観測行列を初回利用時に一度だけ構築して再利用する。

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

- `xoroshiro128plus_next()`、`well512a_next()`、観測数定数の参照：
  $O(1)$時間、$O(1)$追加空間。
- 各`*_lsb_observation_rank()`：初回だけ観測行列の構築に、状態bit数を
  $N$として$O(N^3/64)$時間と$O(N^2/64)$空間。以後は$O(1)$時間。
- 各`recover_*_state_from_lsb()`：初回だけ上記の構築を行う。
  以後は$O(N^2/64)$時間、$O(N/64)$追加空間。
- 状態の比較と変換：Xoroshiro128+では$O(1)$、WELL512aでは
  $O(16)=O(1)$時間。

## 注意点

入力は0または1だけで、指定個数の連続したLSBでなければならない。
Xoroshiro128+は出力のLSBにcarryが入らないため線形だが、加算結果の
上位bit列にはこの復元法をそのまま適用できない。WELL512aの
`initial_index`は観測から復元する値ではなく、呼び出し側が知っている
巡回配列の位相である。どちらも暗号学的安全性を持つ乱数器ではない。
