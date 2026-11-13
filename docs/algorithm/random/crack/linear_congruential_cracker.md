---
title: Linear Congruential Generator Cracker (線形合同法状態復元)
documentation_of: ../../../../src/algorithm/random/crack/linear_congruential_cracker.hpp
---

パラメータが既知の線形合同法を複製するほか、法と 3 個以上の連続出力から乗数と加数を復元する。

## LinearCongruentialCracker

~~~cpp
LinearCongruentialCracker<Generator> cracker(first_output)
~~~

`first_output` を現在状態として予測器を作る。

- `generator_type`: 対象の生成器型 `Generator`。
- `result_type`: `Generator::result_type`。
- コンストラクタ: `first_output` を最後に観測した出力として保持する。正の法に対して法以上の値を渡すと `std::invalid_argument`。
- `next()` / `operator()()`: 次の出力を返す。
- `current_output()`: 予測器が最後に保持した出力を返す。
- `recovered_seed()`: コンストラクタへ渡した出力の直前状態を返す。乗数が法上で可逆でなければ `nullopt`。

`Generator::modulus == 0` では法を $2^W$ と解釈し、乗数が奇数なら Newton 反復で逆元を求める。

## LinearCongruentialParameters

- `multiplier`: 復元した乗数。
- `increment`: 復元した加数。
- `next(state)`: 復元したパラメータで次状態を返す。

## recover_linear_congruential_parameters

~~~cpp
recover_linear_congruential_parameters<UInt, Modulus>(
    std::span<const UInt> outputs
)
recover_linear_congruential_parameters<UInt, Modulus>(
    const std::array<UInt, N>& outputs
)
~~~

3 個以上の連続出力から `LinearCongruentialParameters<UInt, Modulus>` を返す。`span` 版で出力が 3 個未満なら `nullopt`、`array` 版では `N >= 3` を `static_assert` で検査する。最初の状態差が法上で可逆でなければ `nullopt`。正の法に対して法以上の観測がある場合も `nullopt`。復元後は全入力を再生して検査する。

## LinearCongruentialUnknownModulusCandidate

- `modulus`: 連続出力から得た法の候補。
- `multiplier` / `increment`: 候補の法に対して復元した乗数と加数。
- `next(state)`: 候補パラメータで次状態を返す。

## recover_linear_congruential_unknown_modulus_candidate

~~~cpp
recover_linear_congruential_unknown_modulus_candidate<UInt>(
    std::span<const UInt> outputs
)
recover_linear_congruential_unknown_modulus_candidate(
    const std::array<UInt, N>& outputs
)
~~~

4 個以上の連続した生出力について、通常の整数として差 $d_i=x_{i+1}-x_i$ を取り、$\lvert d_i d_{i+2}-d_{i+1}^2\rvert$ の最大公約数を法の候補にする。候補が `UInt` に収まり、全観測より大きく、可逆な状態差から乗数を求められ、全観測を再生できた場合に候補を返す。積と差は最大 129 bit になるため、符号付きオーバーフローを使わない固定幅 129 bit の演算で処理する。

不変量の最大公約数は真の法の倍数になり得るため、戻り値は一意性を保証しない。出力を増やすと余分な共通因子が消えやすい。出力が 4 個未満、最大公約数が 0、型に収まらない、可逆な差がない、または再生検査に失敗する場合は `nullopt`。$2^W$ 法は `UInt` で正の法として表せないため、この API の対象外である。

## 時間計算量

- コンストラクタ、型別名の参照、`next`、`operator()`、`current_output`、各公開データメンバの参照、各パラメータ型の `next`: $O(1)$
- `recovered_seed`: $O(\log M)$。$2^W$ 法では $O(\log W)$
- `recover_linear_congruential_parameters`: 出力数を $K$ として、$M>0$ では $O(K+\log M)$、$M=0$ では $O(K+\log W)$
- `recover_linear_congruential_unknown_modulus_candidate`: $W$ ビット型では $O(KW^2)$ ビット演算。$W\le64$ を固定幅整数の基本演算と数える場合は $O(K)$

追加空間計算量はすべて $O(1)$ である。観測列は `span` で受け取るため、`std::vector` の観測列も `std::span<const UInt>(outputs)` として渡せ、復元時にコピーしない。

## 注意点

- 完全な内部状態が連続して出力されることを前提とする。値域変換後や一部のビットだけの出力には適用できない。
- 既知法の API では法が既知であり、法が正の場合は各観測値が法未満でなければならない。
- 生成値の型は 64 bit 以下の符号なし整数、法は $0$ または $2$ 以上とする（`static_assert`）。
- シードを一意に復元するには $\gcd(A,M)=1$、$2^W$ 法では $A$ が奇数であることが必要である。非可逆でも、最後の出力から次の値は予測できる。
- `recovered_seed()` が返すのは観測出力の直前状態であり、正規化前に渡したシードと一致しない場合がある。

使用例:

~~~cpp
std::vector<std::uint32_t> outputs{1, 6, 31, 59};
auto parameters = recover_linear_congruential_parameters<std::uint32_t, 97>(
    std::span<const std::uint32_t>(outputs)
);
~~~
