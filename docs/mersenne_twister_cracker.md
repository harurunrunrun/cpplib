---
title: Mersenne Twister State Cracker (メルセンヌツイスター状態復元)
documentation_of: ../src/algorithm/random/crack/mersenne_twister_cracker.hpp
---

連続した完全出力のテンパリングを逆変換し、Mersenne Twisterの内部状態列を復元する。

## MersenneTwisterCracker

~~~cpp
MersenneTwisterCracker<
    UInt, WordSize, StateSize, ShiftSize, MaskBits, XorMask,
    TemperingU, TemperingD, TemperingS, TemperingB,
    TemperingT, TemperingC, TemperingL
> cracker(outputs);
~~~

<code>UInt</code> は符号なし整数、<code>WordSize</code> と
<code>StateSize</code> は1以上。<code>ShiftSize</code> は1以上
<code>StateSize</code> 以下、各テンパリングのシフト量は1以上
<code>WordSize</code> 以下。

<code>outputs</code> は <code>std::array</code> で、要素数は
<code>StateSize</code> 以上。先頭 <code>StateSize</code> 語から状態を作り、
余分な語は再生して整合性を検査する。<code>WordSize</code> を超える上位bitが
立っている場合、または整合性検査に失敗した場合は <code>std::invalid_argument</code>。

- <code>result_type</code>：生成値に使う <code>UInt</code>。
- <code>state_type</code>：長さ <code>StateSize</code> の状態配列。
- コンストラクタ：連続出力から状態を復元し、余分な出力があれば照合する。
- <code>next()</code> / <code>operator()()</code>：次の完全出力を返す。
- <code>discard(count)</code>：<code>count</code> 出力分進める。
- <code>state()</code>：次の予測に必要なテンパリング前の状態を古い順で返す。
- <code>min()</code> / <code>max()</code>：下限0と、<code>WordSize</code> bitがすべて1の上限を返す。

<code>Mt19937Cracker</code> は32bit・624語、
<code>Mt19937_64Cracker</code> は64bit・312語の別名。

## 時間計算量

状態語数を $N$、追加検査語数を $K$ とする。

- コンストラクタ：$O(N+K)$ 時間・$O(N)$ 保存領域
- <code>next</code> / <code>operator()</code>：$O(1)$
- <code>discard(count)</code>：$O(count)$
- <code>state()</code>：$O(N)$ 時間・$O(N)$ 戻り値領域
- <code>min</code> / <code>max</code>：$O(1)$
- 型別名の参照：$O(1)$

各生成操作の追加空間計算量は $O(1)$。

## 注意点

観測開始位置は624語blockの境界でなくてもよい。分布変換後、一部bitだけ、
呼出し欠落を含む列には適用できない。復元するのは内部状態であり、最初に渡された
整数seedや <code>seed_seq</code> ではない。暗号用途の乱数には使用しない。
