---
title: GF(2) Linear State Cracker (GF(2)線形状態復元)
documentation_of: ../src/algorithm/random/gf2_linear_state_cracker.hpp
---

$GF(2)$ 上で線形な状態遷移と1bit出力から、連続する出力bitを使って
内部状態を復元する。XOR、shift、rotateだけで構成された遷移などに使える。

## `Gf2LinearStateCracker<StateBits>`

~~~cpp
template<std::size_t StateBits>
class Gf2LinearStateCracker;
~~~

状態を下位bitから並べた
`std::array<std::uint64_t, (StateBits + 63) / 64>`として扱う。

### コンストラクタ

~~~cpp
template<class Step>
explicit Gf2LinearStateCracker(Step step);
~~~

`step(state, output_index)`は状態を1回進め、観測されるbitを`bool`で返す。
`state`と戻り値の両方が入力状態に対して$GF(2)$上で線形でなければならない。
`output_index`は0から始まるため、巡回配列の添字などの既知の外部位相も
表現できる。各基底状態を実行して観測行列を作り、Gauss-Jordan消去する。

### `state_bits`, `word_count`, `state_type`

状態bit数、状態を格納する64bit語数、状態型を表す公開メンバ。

### `rank()`

~~~cpp
std::size_t rank() const noexcept;
~~~

連続`StateBits`出力から作った観測行列のrankを返す。

### `uniquely_recoverable()`

~~~cpp
bool uniquely_recoverable() const noexcept;
~~~

観測行列が満rankで、状態を一意に復元できるかを返す。

### `recover()`

~~~cpp
state_type recover(
    std::span<const std::uint8_t> consecutive_output_bits
) const;
~~~

古い順に並べた連続`StateBits`個の0/1から、最初の出力を生成する直前の
状態を返す。個数が異なるか0/1以外を含む場合は`std::invalid_argument`、
観測行列が満rankでない場合は`std::logic_error`を送出する。

## 時間計算量

`N = StateBits`、64bit語上の演算を1回と数える。

- コンストラクタ：遷移の計算量を$T$として
  $O(N^2T + N^3/64)$時間、$O(N^2/64)$追加空間。
- `rank()`、`uniquely_recoverable()`、各定数・型の参照：$O(1)$時間、
  $O(1)$追加空間。
- `recover()`：$O(N^2/64)$時間、$O(N/64)$追加空間。

## 注意点

加算の上位bit、乗算、carryを含む遷移などは一般には$GF(2)$線形ではない。
線形でない`step`を渡しても検出はできず、復元結果は保証されない。
出力の欠落や順序の入れ替えがある場合にも適用できない。
