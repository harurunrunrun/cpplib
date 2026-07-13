---
title: Xorshift Engines (Xorshift乱数生成器)
documentation_of: ../src/algorithm/random/xorshift.hpp
---

XORとbit shiftで状態を更新する固定状態乱数生成器。

## XorShift32

~~~cpp
XorShift32()
explicit XorShift32(std::uint32_t seed)
~~~

Marsagliaの $(13,17,5)$ 遷移を使う32bit版。

- <code>result_type</code>：<code>std::uint32_t</code>。
- <code>default_seed</code>：既定seed <code>2463534242</code>。
- 各コンストラクタ：既定seedまたは指定したseedで構築する。
- <code>seed(value)</code>：状態を <code>value</code> へ戻す。引数を省略した場合は既定seedを使う。0なら <code>std::invalid_argument</code>。
- <code>operator()()</code>：状態を3回のXOR shiftで更新し、更新後の値を返す。
- <code>state()</code>：現在の32bit状態を返す。
- <code>min()</code> / <code>max()</code>：生成範囲の下限1と上限 <code>UINT32_MAX</code> を返す。

## XorShift64

~~~cpp
XorShift64()
explicit XorShift64(std::uint64_t seed)
~~~

$(13,7,17)$ 遷移を使う64bit版。<code>result_type</code> は
<code>std::uint64_t</code>、<code>default_seed</code> は
<code>88172645463325252</code>。コンストラクタ、<code>seed</code>、
<code>operator()</code>、<code>state</code>、<code>min</code>、<code>max</code> の
意味は <code>XorShift32</code> と同じ。

## XorShift128

~~~cpp
XorShift128()
explicit XorShift128(const std::array<std::uint32_t, 4>& seed)
XorShift128(std::uint32_t x, std::uint32_t y,
            std::uint32_t z, std::uint32_t w)
~~~

4語のMarsaglia Xorshift128。

- <code>result_type</code>：<code>std::uint32_t</code>。
- <code>state_type</code>：<code>std::array&lt;result_type, 4&gt;</code>。
- <code>default_seed</code>：4語の既定状態。
- 各コンストラクタ：既定状態、配列、または4個の整数から構築する。
- <code>seed(state)</code>：4語を置換する。引数を省略した場合は既定状態へ戻す。

<code>seed(state)</code> は
全語0なら <code>std::invalid_argument</code>。<code>operator()()</code> は
次の32bit値、<code>state()</code> は現在の4語を返す。
<code>min()</code> は0、<code>max()</code> は <code>UINT32_MAX</code>。

## 時間計算量

全コンストラクタ、<code>seed</code>、<code>operator()</code>、
<code>state</code>、<code>min</code>、<code>max</code> は
時間・追加空間計算量ともに $O(1)$。
型別名と静的定数の参照も $O(1)$。

## 注意点

0状態はXorshiftの吸収状態なので受理しない。完全周期はここで固定したパラメータと
非零状態についての性質であり、任意のshift列へ一般化した型ではない。
暗号用途には使用しない。
