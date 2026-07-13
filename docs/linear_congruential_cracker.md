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
復元後は全入力を再生して検査する。

## 時間計算量

- コンストラクタ、型別名の参照、<code>next</code>、<code>operator()</code>、<code>current_output</code>、各公開データメンバの参照、<code>LinearCongruentialParameters::next</code>：$O(1)$
- <code>recovered_seed</code>：$O(\log M)$。$2^W$ 法では $O(\log W)$
- <code>recover_linear_congruential_parameters</code>：出力数を $K$ として、$M>0$ では $O(K+\log M)$、$M=0$ では $O(K+\log W)$

追加空間計算量は $O(1)$。

## 注意点

完全な内部状態が連続して出力され、法が既知であることを前提とする。
生成値の型は64bit以下の符号なし整数、法は0または2以上とする。法が正の場合、
各観測値は法未満でなければならない。
seedの一意復元には $\gcd(A,M)=1$、$2^W$ 法では $A$ が奇数であることが必要。
<code>recovered_seed()</code> が返すのは観測出力の直前状態であり、正規化前に
渡したseedと一致しない場合がある。非可逆でも最後の出力から次値は予測できる。
値域変換後や一部bitだけの出力には
適用できない。
