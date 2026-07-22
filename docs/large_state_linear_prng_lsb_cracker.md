---
title: Large-State Linear PRNG LSB Cracker (大状態線形PRNGのLSB状態復元)
documentation_of: ../src/algorithm/random/crack/large_state_linear_prng_lsb_cracker.hpp
---

Xoshiro512+とXoroshiro1024*の連続出力を2で割った余りだけから、観測直前の
内部状態を復元する。観測行列は初回利用時に構築し、同じprocess内の後続queryで
再利用する。

## `Xoshiro512PlusState`

```cpp
struct Xoshiro512PlusState{
    std::array<std::uint64_t, 8> words;
};
```

Xoshiro512+ 1.0の512 bit状態を保持する。`operator==`を使用できる。

## `xoshiro512plus_next`

```cpp
std::uint64_t xoshiro512plus_next(Xoshiro512PlusState& state) noexcept;
```

`state.words[0] + state.words[2]`を返し、$(11,21)$遷移で状態を1回進める。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$
- 例外を送出しない

## Xoshiro512+の観測情報

```cpp
inline constexpr std::size_t xoshiro512plus_lsb_observation_count = 512;
std::size_t xoshiro512plus_lsb_observation_rank();
```

状態を一意に復元するために必要な連続LSB数と、512行の観測行列のrankを返す。
rankは512である。

- 観測数定数の参照: $O(1)$ 時間・$O(1)$ 追加領域
- rankの初回呼出し: $O(512^3/64)$ 時間・$O(512^2/64)$ 領域
- 同じprocess内の2回目以降: $O(1)$ 時間・$O(1)$ 追加領域

## `recover_xoshiro512plus_state_from_lsb`

```cpp
Xoshiro512PlusState recover_xoshiro512plus_state_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb
);
```

古い順に並べた512個の連続LSBから、最初の出力を得る直前の状態を返す。

- 初回呼出し: 観測行列構築を含め $O(512^3/64)$ 時間、
  $O(512^2/64)$ 保存領域
- 行列構築後: $O(512^2/64)$ 時間、$O(512/64)$ 追加領域
- 要素数が512でない、または要素に0、1以外を含む場合は
  `std::invalid_argument`

## `Xoroshiro1024StarState`

```cpp
struct Xoroshiro1024StarState{
    std::array<std::uint64_t, 16> words;
    std::size_t index;
};
```

Xoroshiro1024* 1.0の1024 bit状態と、巡回配列の現在位置を保持する。
`operator==`を使用できる。

## `xoroshiro1024star_next`

```cpp
std::uint64_t xoroshiro1024star_next(
    Xoroshiro1024StarState& state
);
```

次の状態語に奇数 `0x9e3779b97f4a7c13` を乗じた値を返し、$(25,27,36)$遷移で
状態と`index`を1回進める。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$
- `index`は呼出前に`[0,16)`でなければならない

## Xoroshiro1024*の観測情報

```cpp
inline constexpr std::size_t xoroshiro1024star_lsb_observation_count = 1024;
std::size_t xoroshiro1024star_lsb_observation_rank();
```

状態を一意に復元するために必要な連続LSB数と、1024行の観測行列のrankを返す。
rankは1024である。

- 観測数定数の参照: $O(1)$ 時間・$O(1)$ 追加領域
- rankの初回呼出し: $O(1024^3/64)$ 時間・$O(1024^2/64)$ 領域
- 同じprocess内の2回目以降: $O(1)$ 時間・$O(1)$ 追加領域

## `recover_xoroshiro1024star_state_from_lsb`

```cpp
Xoroshiro1024StarState recover_xoroshiro1024star_state_from_lsb(
    std::span<const std::uint8_t> consecutive_output_lsb,
    std::size_t initial_index = 0
);
```

既知の`initial_index`から得た1024個の連続LSBを古い順に受け取り、最初の出力を
得る直前の物理的な`words`配置と`index`を返す。

- 初回呼出し: 観測行列構築を含め $O(1024^3/64)$ 時間、
  $O(1024^2/64)$ 保存領域
- 行列構築後: $O(1024^2/64)$ 時間、$O(1024/64)$ 追加領域
- 要素数が1024でない、要素に0、1以外を含む、または
  `initial_index >= 16`の場合は`std::invalid_argument`

## 注意点

- `+`出力のLSBには下位bitからのcarryが入らないため、Xoshiro512+のLSBは
  GF(2)上の線形観測になる。Xoroshiro1024*の乗数は奇数なので、積のLSBは
  乗算前の語のLSBと等しい。
- 上位出力bitには加算または乗算のcarryがあるため、このGF(2)復元をそのまま
  適用できない。
- Xoroshiro1024*の`index`は出力LSBから求める値ではなく、呼出側が既知として
  渡す巡回配列の位相である。
- 通常の乱数生成では全語0の状態をseedにしてはならない。復元関数は線形写像の
  契約を保つため、全語0の観測も受理する。
- どちらも暗号学的安全性を持たない。
