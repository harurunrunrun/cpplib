---
title: Subtract-with-Carry State Cracker (キャリー付き減算法状態復元)
documentation_of: ../src/algorithm/random/subtract_with_carry_cracker.hpp
---

<code>std::subtract_with_carry_engine</code> の加工されていない連続出力から、
carryを含む内部状態を復元する。整数seedを有限候補集合から全探索するAPIも備える。

## SubtractWithCarryCracker

~~~cpp
SubtractWithCarryCracker<UInt, WordSize, ShortLag, LongLag> cracker(outputs);
~~~

<code>UInt</code> は64bit以下の符号なし整数、
<code>WordSize</code> は1以上かつ <code>UInt</code> のbit数以下、
<code>ShortLag</code> と <code>LongLag</code> は
$0<\mathtt{ShortLag}<\mathtt{LongLag}$ を満たす。

- <code>result_type</code>：生成値に使う <code>UInt</code>。
- <code>state_type</code>：長さ <code>LongLag</code> の状態配列。
- <code>SubtractWithCarryCracker(outputs)</code>：長さ <code>Count</code> の
  <code>std::array&lt;result_type, Count&gt;</code> から状態とcarryを復元する。
  <code>Count</code> は <code>LongLag + 1</code> 以上。余分な出力もすべて
  漸化式と照合して消費するため、成功後は <code>outputs.back()</code> の直後を指す。
  語のbit幅違反または途中で漸化式と一致しない場合は
  <code>std::invalid_argument</code>。
- <code>next()</code>：次の生出力を返し、状態を1回進める。
- <code>operator()()</code>：<code>next()</code> と同じ。
- <code>discard(count)</code>：生出力を返さずに <code>count</code> 回進める。
- <code>carry()</code>：次の遷移で使う現在のcarryを返す。
- <code>state()</code>：直近 <code>LongLag</code> 語を古い順に返す。
- <code>min()</code>：0を返す。
- <code>max()</code>：<code>WordSize</code> bitがすべて1の値を返す。

<code>Ranlux24BaseCracker</code> は
<code>SubtractWithCarryCracker&lt;std::uint_fast32_t, 24, 10, 24&gt;</code>、
<code>Ranlux48BaseCracker</code> は
<code>SubtractWithCarryCracker&lt;std::uint_fast64_t, 48, 5, 12&gt;</code>
の型別名。

## SubtractWithCarrySeedRecoveryStatus

- <code>none</code>：一致するseedがない。
- <code>unique</code>：一致するseedがちょうど1個。
- <code>ambiguous</code>：一致するseedが2個以上。

## SubtractWithCarrySeedRecovery

- <code>result_type</code>：<code>Engine::result_type</code>。
- <code>status</code>：探索結果を表す
  <code>SubtractWithCarrySeedRecoveryStatus</code>。
- <code>seed</code>：見つかったseed。<code>status == unique</code> のときだけ有効。
- <code>predictor</code>：観測列を消費した直後の生成器。
  <code>status == unique</code> のときだけ有効。
- <code>unique()</code>：一致seedが1個だけならtrue。
- <code>recovered_seed()</code>：一意なseedを
  <code>std::optional</code> で返す。一意でなければ <code>std::nullopt</code>。
- <code>next()</code>：一意な <code>predictor</code> の次出力を返す。
  <code>status != unique</code> では <code>std::logic_error</code>。

## recover_subtract_with_carry_seed_candidates

~~~cpp
recover_subtract_with_carry_seed_candidates<Engine>(
    outputs, discard_count, seed_first, candidate_count
)
~~~

<code>seed_first</code> から連続する <code>candidate_count</code> 個、
すなわち <code>seed_first + offset</code>
（<code>0 &lt;= offset &lt; candidate_count</code>）を全探索する。
各候補で生成器を初期化し、<code>discard_count</code> 回進めてから
長さ <code>Count</code> の <code>outputs</code> と照合する。
一意な場合の <code>predictor</code> は全観測を消費した直後を指す。

<code>candidate_count == 0</code> なら <code>none</code> を返す。
最後の候補が <code>Engine::result_type</code> の最大値を越える場合は
<code>std::invalid_argument</code>。加算で終端を作らないため、
最大値そのものも、例えば <code>seed_first == max</code> かつ
<code>candidate_count == 1</code> として安全に探索できる。
<code>Count</code> は1以上で、<code>Engine::result_type</code> は
<code>unsigned long long</code> 以下のbit数を持つ符号なし整数とする。

## recover_subtract_with_carry_seed_in_range

~~~cpp
recover_subtract_with_carry_seed_in_range<Engine>(
    outputs, discard_count, seed_first, seed_last
)
~~~

従来互換の半開区間 <code>[seed_first, seed_last)</code> を全探索する。
候補数を <code>seed_last - seed_first</code> として
<code>recover_subtract_with_carry_seed_candidates</code> に委譲する。
<code>seed_last &lt; seed_first</code> では <code>std::invalid_argument</code>。
同じ整数型の排他的終端では最大値の次を表現できないため、
最大seedを含める場合は候補数APIを使う。

## 時間計算量

<code>Count</code> を状態復元へ渡した観測数、
<code>D</code> をseed候補数、<code>K</code> をseed探索の観測数、
<code>Q</code> を <code>discard_count</code> とする。

- <code>SubtractWithCarryCracker(outputs)</code>：$O(Count)$ 時間、
  $O(\mathtt{LongLag})$ 保存領域
- <code>next()</code> / <code>operator()()</code>：$O(1)$
- <code>discard(count)</code>：$O(count)$
- <code>carry()</code>：$O(1)$
- <code>state()</code>：$O(\mathtt{LongLag})$ 時間・戻り値領域
- <code>min()</code> / <code>max()</code>：$O(1)$
- <code>unique()</code> / <code>recovered_seed()</code> /
  <code>SubtractWithCarrySeedRecovery::next()</code>：$O(1)$
- 公開型別名・enum値・データメンバの参照：$O(1)$
- <code>recover_subtract_with_carry_seed_candidates</code> /
  <code>recover_subtract_with_carry_seed_in_range</code>：
  $O(D(\mathtt{LongLag}+Q+K))$ 時間、
  $O(\mathtt{LongLag})$ 追加領域

## 注意点

- 復元には最低 <code>LongLag + 1</code> 個の、同じ生成器から得た連続生出力が必要。
  追加観測を渡すと誤った列を早期に検出し、復元後の位置もその分だけ進む。
- seed探索で得られる一意性は、指定した候補集合と
  <code>discard_count</code> の中だけの一意性。既知のskip数を正確に渡す。
- <code>ranlux24</code> / <code>ranlux48</code> は
  <code>std::discard_block_engine</code> による間引き後の列なので直接対象にしない。
  生の <code>ranlux24_base</code> / <code>ranlux48_base</code> に使用する。
