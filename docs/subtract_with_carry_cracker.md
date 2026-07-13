---
title: Subtract-with-Carry State Cracker (キャリー付き減算法状態復元)
documentation_of: ../src/algorithm/random/subtract_with_carry_cracker.hpp
---

<code>std::subtract_with_carry_engine</code> の連続生出力から、
carryを含む内部状態を復元する。整数seedを有限候補区間から探す関数も備える。

## SubtractWithCarryCracker

~~~cpp
SubtractWithCarryCracker<UInt, WordSize, ShortLag, LongLag>
    cracker(outputs);
~~~

<code>UInt</code> は64bit以下の符号なし整数で、<code>WordSize</code> はそのbit数以下。
<code>ShortLag</code> と <code>LongLag</code> は $0<\mathtt{ShortLag}<\mathtt{LongLag}$ を満たす。

<code>outputs</code> は長さ <code>LongLag + 1</code> の
<code>std::array</code>。語のbit幅違反または漸化式と一致しない列では
<code>std::invalid_argument</code>。

- <code>result_type</code>：生成値に使う <code>UInt</code>。
- <code>state_type</code>：長さ <code>LongLag</code> の状態配列。
- コンストラクタ：連続出力から状態とcarryを復元する。
- <code>next()</code> / <code>operator()()</code>：次の生出力を返す。
- <code>discard(count)</code>：<code>count</code> 出力分進める。
- <code>carry()</code>：現在のcarryを返す。
- <code>state()</code>：直近 <code>LongLag</code> 語を古い順で返す。
- <code>min()</code> / <code>max()</code>：下限0と、<code>WordSize</code> bitがすべて1の上限を返す。

<code>Ranlux24BaseCracker</code> と <code>Ranlux48BaseCracker</code> は
標準パラメータに対応する別名。

## SubtractWithCarrySeedRecoveryStatus

<code>none</code> は一致なし、<code>unique</code> は一致が1個、
<code>ambiguous</code> は一致が複数あることを表す。

## SubtractWithCarrySeedRecovery

- <code>result_type</code>：<code>Engine::result_type</code>。
- <code>status</code>：探索結果を表す <code>SubtractWithCarrySeedRecoveryStatus</code>。
- <code>seed</code>：見つかったseed。<code>status == unique</code> のときだけ有効。
- <code>predictor</code>：観測列を消費した直後の生成器。<code>status == unique</code> のときだけ有効。
- <code>unique()</code>：一致seedが1個だけならtrue。
- <code>recovered_seed()</code>：一意seed、または <code>nullopt</code>。
- <code>next()</code>：一意な場合に次出力を返す。それ以外では <code>std::logic_error</code>。

## recover_subtract_with_carry_seed_in_range

~~~cpp
recover_subtract_with_carry_seed_in_range<Engine>(
    outputs, discard_count, seed_first, seed_last
)
~~~

整数seed候補を半開区間 <code>[seed_first, seed_last)</code> で全探索する。
<code>seed_last &lt; seed_first</code> では <code>std::invalid_argument</code>。観測配列の要素数 <code>Count</code> は1以上でなければならない。

## 時間計算量

- コンストラクタ：$O(\mathtt{LongLag})$ 時間・保存領域
- <code>next</code> / <code>operator()</code> / <code>carry</code>：$O(1)$
- <code>discard(count)</code>：$O(count)$
- <code>state()</code>：$O(\mathtt{LongLag})$ 時間・$O(\mathtt{LongLag})$ 戻り値領域
- <code>min</code> / <code>max</code>、<code>unique</code>、<code>recovered_seed</code>、<code>SubtractWithCarrySeedRecovery::next</code>、公開データメンバと型別名の参照：$O(1)$
- seed候補数を $D$、観測数を $K$、<code>discard_count</code> を $Q$ とした範囲探索：$O(D(\mathtt{LongLag}+Q+K))$

探索結果と探索中の追加空間計算量は $O(\mathtt{LongLag})$。

## 注意点

状態復元に必要なのは <code>LongLag + 1</code> 個の加工されていない連続出力。
整数seedは一般に一意でないため、探索結果の <code>status</code> を確認する。
<code>ranlux24</code> / <code>ranlux48</code> は <code>std::discard_block_engine</code> を使うため直接対象にせず、
<code>ranlux24_base</code> / <code>ranlux48_base</code> に使用する。
