---
title: Linear Congruential Generator Cracker (線形合同法状態復元)
documentation_of: ../src/algorithm/random/linear_congruential_cracker.hpp
---

パラメータが既知の線形合同法を複製するほか、法と3個以上の連続出力から
乗数と加数を復元する。

## LinearCongruentialCracker

~~~cpp
LinearCongruentialCracker<Generator> cracker(first_output)
~~~

<code>first_output</code> を現在状態として予測器を作る。

- <code>generator_type</code>：対象の生成器型 <code>Generator</code>。
- <code>result_type</code>：<code>Generator::result_type</code>。
- コンストラクタ：<code>first_output</code> を最後に観測した出力として保持する。
  正の法に対して法以上の値を渡すと <code>std::invalid_argument</code>。
- <code>next()</code> / <code>operator()()</code>：次の出力を返す。
- <code>current_output()</code>：予測器が最後に保持した出力を返す。
- <code>recovered_seed()</code>：コンストラクタへ渡した出力の直前状態を返す。乗数が法上で可逆でなければ <code>nullopt</code>。

<code>Generator::modulus == 0</code> では法を $2^W$ と解釈し、
乗数が奇数ならNewton反復で逆元を求める。

## LinearCongruentialParameters

- <code>multiplier</code>：復元した乗数。
- <code>increment</code>：復元した加数。
- <code>next(state)</code>：復元したパラメータで次状態を返す。

## recover_linear_congruential_parameters

~~~cpp
recover_linear_congruential_parameters<UInt, Modulus>(outputs)
~~~

3個以上の連続出力から <code>LinearCongruentialParameters&lt;UInt, Modulus&gt;</code> を返す。
最初の状態差が法上で可逆でなければ <code>nullopt</code>。
正の法に対して法以上の観測がある場合も <code>nullopt</code>。
復元後は全入力を再生して検査する。

## LinearCongruentialUnknownModulusCandidate

- <code>modulus</code>：連続出力から得た法の候補。
- <code>multiplier</code> / <code>increment</code>：候補の法に対して復元した乗数と加数。
- <code>next(state)</code>：候補パラメータで次状態を返す。

## recover_linear_congruential_unknown_modulus_candidate

~~~cpp
recover_linear_congruential_unknown_modulus_candidate<UInt>(
    std::span<const UInt> outputs
)
recover_linear_congruential_unknown_modulus_candidate(
    const std::array<UInt, N>& outputs
)
~~~

4個以上の連続した生出力について、通常の整数として差
$d_i=x_{i+1}-x_i$ を取り、
$|d_i d_{i+2}-d_{i+1}^2|$ の最大公約数を法の候補にする。
候補が <code>UInt</code> に収まり、全観測より大きく、可逆な状態差から
乗数を求められ、全観測を再生できた場合に候補を返す。
積と差は最大129bitになるため、符号付きoverflowを使わない固定幅129bit演算で処理する。

不変量の最大公約数は真の法の倍数になり得るため、戻り値は一意性を保証しない。
出力を増やすと余分な共通因子が消えやすい。最大公約数が0、型に収まらない、
可逆な差がない、または再生検査に失敗する場合は <code>nullopt</code>。
$2^W$ 法は <code>UInt</code> で正の法として表せないため、このAPIの対象外。

## 時間計算量

- コンストラクタ、型別名の参照、<code>next</code>、<code>operator()</code>、<code>current_output</code>、各公開データメンバの参照、各パラメータ型の <code>next</code>：$O(1)$
- <code>recovered_seed</code>：$O(\log M)$。$2^W$ 法では $O(\log W)$
- <code>recover_linear_congruential_parameters</code>：出力数を $K$ として、$M>0$ では $O(K+\log M)$、$M=0$ では $O(K+\log W)$
- <code>recover_linear_congruential_unknown_modulus_candidate</code>：$W$ bit型では $O(KW^2)$ bit演算。$W\le64$ を固定幅整数の基本演算と数える場合は $O(K)$

追加空間計算量はすべて $O(1)$。

## 注意点

完全な内部状態が連続して出力されることを前提とする。
既知法APIでは法が既知であり、法が正の場合は各観測値が法未満でなければならない。
生成値の型は64bit以下の符号なし整数、法は0または2以上とする。
seedの一意復元には $\gcd(A,M)=1$、$2^W$ 法では $A$ が奇数であることが必要。
<code>recovered_seed()</code> が返すのは観測出力の直前状態であり、正規化前に
渡したseedと一致しない場合がある。非可逆でも最後の出力から次値は予測できる。
値域変換後や一部bitだけの出力には適用できない。
