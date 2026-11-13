---
title: GF(2) Linear State Cracker (GF(2)線形状態復元)
documentation_of: ../../../../src/algorithm/random/crack/gf2_linear_state_cracker.hpp
---

$GF(2)$ 上で線形な状態遷移と 1 bit の出力から、連続する出力ビットを使って内部状態を復元する。XOR、シフト、ローテートだけで構成された遷移などに使える。

## `Gf2LinearStateCracker<StateBits>`

~~~cpp
template<std::size_t StateBits = static_cast<std::size_t>(-1)>
class Gf2LinearStateCracker;
~~~

状態を下位ビットから並べた `std::array<std::uint64_t, (StateBits + 63) / 64>` として扱う。`StateBits` を省略すると、状態ビット数を実行時に受け取る可変長版になる（後述）。

### コンストラクタ

~~~cpp
template<class Step>
explicit Gf2LinearStateCracker(Step step);
~~~

`step(state, output_index)` は状態を 1 回進め、観測されるビットを `bool` で返す。`state` と戻り値の両方が、入力状態に対して $GF(2)$ 上で線形でなければならない。`output_index` は 0 から始まるため、巡回配列の添字などの既知の外部位相も表現できる。各基底状態について遷移を実行して観測行列を作り、Gauss-Jordan 消去を行う。

### `state_bits`, `word_count`, `state_type`

状態のビット数、状態を格納する 64 bit 語の数、状態の型を表す公開メンバである。

### `rank()`

~~~cpp
std::size_t rank() const noexcept;
~~~

連続する `StateBits` 個の出力から作った観測行列のランクを返す。

### `uniquely_recoverable()`

~~~cpp
bool uniquely_recoverable() const noexcept;
~~~

観測行列がフルランクで、状態を一意に復元できるかを返す。

### `recover()`

~~~cpp
state_type recover(
    std::span<const std::uint8_t> consecutive_output_bits
) const;
~~~

古い順に並べた連続する `StateBits` 個の 0/1 から、最初の出力を生成する直前の状態を返す。個数が異なるか 0/1 以外を含む場合は `std::invalid_argument` を、観測行列がフルランクでない場合は `std::logic_error` を送出する。

## 可変長版

`Gf2LinearStateCracker<>` は状態ビット数をコンストラクタで受け取り、状態と消去用の行列を `std::vector` に保存する。`state_type` は `std::vector<std::uint64_t>` で、`state_bits` と `word_count` は非 static なメンバになる。状態ビット数が $0$ などの不正な値なら `std::invalid_argument` を送出する。

~~~cpp
std::size_t state_bits = 16;
auto step = [](std::vector<std::uint64_t>& state, std::size_t){
    const bool output = (state[0] & 1ULL) != 0;
    state[0] = (state[0] >> 1) | ((state[0] & 1ULL) << 15);
    return output;
};
Gf2LinearStateCracker<> cracker(state_bits, step);
std::vector<std::uint8_t> observations(state_bits, 0);
auto recovered = cracker.recover(observations);
~~~

## 時間計算量

$N$ を状態ビット数、$W=\lceil N/64\rceil$ とし、64 bit 語の演算を 1 回と数える。1 回の `step` の時間を $T$ とする。

- コンストラクタ: $O(N^2T + N^2W)$ 時間、$O(NW)$ 追加空間。
- `rank()`、`uniquely_recoverable()`、各定数・型の参照: $O(1)$ 時間、$O(1)$ 追加空間。
- `recover()`: $O(NW)$ 時間、$O(W)$ 追加空間。

## 注意点

- 加算の上位ビット、乗算、繰り上がりを含む遷移などは、一般には $GF(2)$ 上で線形ではない。線形でない `step` を渡しても検出できず、復元結果は保証されない。
- 出力の欠落や順序の入れ替えがある場合には適用できない。
