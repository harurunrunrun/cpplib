---
title: Philox Engine (Philoxカウンタベース乱数生成器)
documentation_of: ../src/algorithm/random/philox_engine.hpp
---

現行C++ draftの <code>philox_engine</code> と同じcounter・key・round定義を
C++20で使用する固定配列実装。

## PhiloxEngine

~~~cpp
PhiloxEngine<UInt, WordSize, WordCount, RoundCount, Constants...>
~~~

<code>UInt</code> は64bit以下の符号なし整数、<code>WordSize</code> は1以上かつ
<code>UInt</code> のbit数以下、<code>WordCount</code> は2または4、
<code>RoundCount</code> は1以上。<code>Constants</code> は乗数とround定数を
交互に <code>WordCount</code> 個指定する。

## 公開型・定数

- <code>result_type</code>：生成値に使う <code>UInt</code>。
- <code>counter_type</code>：長さ <code>WordCount</code> のcounter配列。
- <code>key_type</code>：長さ <code>WordCount / 2</code> のkey配列。
- <code>block_type</code>：長さ <code>WordCount</code> の出力block配列。
- <code>word_size</code> / <code>word_count</code> / <code>round_count</code>：対応するテンプレート引数。
- <code>default_seed</code>：既定seed <code>20111115</code>。
- <code>multipliers</code> / <code>round_consts</code>：<code>Constants</code> から取り出した乗数列とround定数列。

## 構築とseed

- 既定コンストラクタ：<code>default_seed</code> を使う。
- <code>PhiloxEngine(value)</code> / <code>seed(value)</code>：先頭keyを <code>value</code>、残りのkeyとcounterを0にする。引数を省略した <code>seed()</code> は既定seedを使う。
- <code>PhiloxEngine(seed_sequence)</code> / <code>seed(seed_sequence)</code>：<code>SeedSequence::generate</code> で全keyを作る。

## counterと生成

- <code>set_counter(counter)</code>：公開配列を上位語から並べたcounterとして設定し、次の呼出しをblock先頭にする。
- <code>counter()</code>：次にblock生成へ使うcounterを上位語から返す。
- <code>key()</code>：現在のkey配列を返す。
- <code>index()</code>：出力block内で最後に返した位置を返す。<code>WordCount - 1</code> のとき、次の呼出しで新しいblockを生成する。
- <code>operator()()</code>：blockの次の語を返す。必要ならPhilox roundを実行してcounterを1増やす。
- <code>discard(count)</code>：完全blockはcounter加算で飛ばし、端のblockだけ生成する。
- <code>operator==</code>：将来の出力列を決めるcounter・key・indexを比較する。
- <code>operator&lt;&lt;</code>：標準の textual representation と同じく、key <code>K[0..N/2)</code>、内部 counter <code>X[0..N)</code>、<code>index</code> の順に10進数で出力する。stream の flags と fill は呼出し後に復元する。
- <code>operator&gt;&gt;</code>：上記の状態を読み戻す。各 word が $[0,2^w)$、index が $[0,N)$ にあることを検査し、途中 block なら直前 counter から buffer を復元する。不正入力では <code>failbit</code> を設定して engine を変更せず、stream の flags も復元する。

textual representation の <code>X[0]</code> は最下位語である。
<code>set_counter</code> / <code>counter()</code> の公開配列はこれと逆順なので注意する。

- <code>min()</code> / <code>max()</code>：1語の出力範囲を返す。

<code>Philox4x32</code> と <code>Philox4x64</code> は10 roundの
標準パラメータを使う別名。

## 時間計算量

$N=\mathtt{WordCount}$、$R=\mathtt{RoundCount}$ とする。

- 既定・整数seedコンストラクタ、<code>seed(value)</code>：$O(N)$
- SeedSequenceコンストラクタ、<code>seed(seed_sequence)</code>：$O(N)$ と <code>SeedSequence::generate</code> の計算量
- <code>set_counter</code>、<code>counter</code>、<code>key</code>：$O(N)$
- <code>index</code>、<code>min</code>、<code>max</code>、整数の静的定数の参照：$O(1)$
- <code>multipliers</code> / <code>round_consts</code>：要素参照は $O(1)$、配列全体のコピーは $O(N)$
- <code>operator()</code>：最悪 $O(RN)$、ならし $O(R)$
- <code>discard</code>：$O(N+RN)$。完全block数には依存しない
- <code>operator==</code>：$O(N)$
- <code>operator&lt;&lt;</code>：$O(N)$
- <code>operator&gt;&gt;</code>：最悪 $O(RN)$、block 境界では $O(N)$

保存領域は $O(N)$。<code>operator()</code> と <code>discard</code> の
最悪追加空間計算量は $O(N)$、それ以外の操作は戻り値を除いて $O(1)$。

stream 入出力の追加領域は $O(N)$。

## 注意点

<code>set_counter</code> の配列は上位語が先頭で、内部counterの最下位語は
末尾要素に対応する。<code>Constants</code> の並びは
乗数、round定数、乗数、round定数の順。統計的乱数生成器であり、
暗号用途には使用しない。
64bit word の乗算には GCC/Clang の <code>unsigned __int128</code> 拡張を使う。
stream 演算子は hosted 環境向けである。
