---
title: Linear Congruential Generator (線形合同法乱数生成器)
documentation_of: ../src/algorithm/random/engine/linear_congruential_generator.hpp
---

$x_{i+1}=Ax_i+C\pmod M$ で状態を更新する線形合同法乱数生成器。

~~~cpp
LinearCongruentialGenerator<UInt, A, C, M> engine(seed)
~~~

<code>UInt</code> は64bit以下の符号なし整数。$M$ は0または2以上で、
$M>0$ では $A,C<M$ が必要。<code>M == 0</code> は $2^W$（$W$ は <code>UInt</code> のbit数）を法とし、
符号なし整数の桁あふれを利用する。

## 公開API

- <code>result_type</code>：生成値と内部状態に使う型 <code>UInt</code>。
- <code>multiplier</code> / <code>increment</code> / <code>modulus</code>：それぞれ $A,C,M$ を表す静的定数。
- <code>default_seed</code>：既定seedの1。
- 既定コンストラクタ：seed 1で構築する。
- <code>LinearCongruentialGenerator(seed)</code> / <code>seed(value)</code>：$M>0$ では <code>value % M</code> を状態にする。$C=0$ かつ正規化後が0なら状態1を使う。引数を省略した <code>seed()</code> は既定seedへ戻す。
- <code>transition(state)</code>：1回遷移した状態を返す。生成器自身は変更しない。
- <code>operator()()</code>：1回遷移し、更新後の状態を返す。
- <code>discard(count)</code>：<code>count</code> 回分をアフィン変換の二乗法で飛ばす。
- <code>state()</code>：現在状態を返す。
- <code>min()</code> / <code>max()</code>：生成範囲を返す。$C\ne0$ の <code>min()</code> は0。$C=0$ では、$M=0$ かつ $A$ が奇数の場合、または $M>0$ かつ $\gcd(A,M)=1$ の場合に1、それ以外は0。
  <code>max()</code> は $M=0$ なら <code>std::numeric_limits&lt;UInt&gt;::max()</code>、$M>0$ なら $M-1$。

<code>MinStdRand0</code> は $(A,C,M)=(16807,0,2147483647)$、
<code>MinStdRand</code> は $(48271,0,2147483647)$ の別名。

## 時間計算量

- 各コンストラクタ、<code>seed</code>、<code>transition</code>、<code>operator()</code>、<code>state</code>、<code>max</code>、各静的定数の参照：$O(1)$
- <code>min()</code>：最悪 $O(\log M)$。$C\ne0$ または $M=0$ では $O(1)$
- <code>discard(count)</code>：$O(\log count)$

追加空間計算量はすべて $O(1)$。

## 注意点

64bit積は <code>unsigned __int128</code> で計算してから剰余を取る。
内部状態をそのまま出力し、上位bitの抽出や値域変換は行わない。
暗号用途には使用しない。
