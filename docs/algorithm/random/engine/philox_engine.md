---
title: Philox Engine (Philoxカウンタベース乱数生成器)
documentation_of: ../../../../src/algorithm/random/engine/philox_engine.hpp
---

現行 C++ draft の `philox_engine` と同じカウンタ・キー・ラウンド定義を C++20 で使用する固定配列実装。

## PhiloxEngine

~~~cpp
PhiloxEngine<UInt, WordSize, WordCount, RoundCount, Constants...>
~~~

`UInt` は 64bit 以下の符号なし整数、`WordSize` は 1 以上かつ `UInt` のビット数以下、`WordCount` は 2 または 4、`RoundCount` は 1 以上。`Constants` は乗数とラウンド定数を交互に `WordCount` 個指定する。

## 公開型・定数

- `result_type`：生成値に使う `UInt`。
- `counter_type`：長さ `WordCount` のカウンタ配列。
- `key_type`：長さ `WordCount / 2` のキー配列。
- `block_type`：長さ `WordCount` の出力ブロック配列。
- `word_size` / `word_count` / `round_count`：対応するテンプレート引数。
- `default_seed`：既定シード `20111115`。
- `multipliers` / `round_consts`：`Constants` から取り出した乗数列とラウンド定数列。

## 構築とシード

- 既定コンストラクタ：`default_seed` を使う。
- `PhiloxEngine(value)` / `seed(value)`：先頭キーを `value`、残りのキーとカウンタを 0 にする。引数を省略した `seed()` は既定シードを使う。
- `PhiloxEngine(seed_sequence)` / `seed(seed_sequence)`：`SeedSequence::generate` で全キーを作る。

## カウンタと生成

- `set_counter(counter)`：公開配列を上位語から並べたカウンタとして設定し、次の呼出しをブロック先頭にする。
- `counter()`：次にブロック生成へ使うカウンタを上位語から返す。
- `key()`：現在のキー配列を返す。
- `index()`：出力ブロック内で最後に返した位置を返す。`WordCount - 1` のとき、次の呼出しで新しいブロックを生成する。
- `operator()()`：ブロックの次の語を返す。必要なら Philox ラウンドを実行してカウンタを 1 増やす。
- `discard(count)`：完全ブロックはカウンタ加算で飛ばし、端のブロックだけ生成する。
- `operator==`：将来の出力列を決めるカウンタ・キー・index を比較する。
- `operator<<`：標準の textual representation と同じく、キー `K[0..N/2)`、内部カウンタ `X[0..N)`、`index` の順に 10 進数で出力する。stream の flags と fill は呼出し後に復元する。
- `operator>>`：上記の状態を読み戻す。各語が $[0,2^w)$、index が $[0,N)$ にあることを検査し、途中ブロックなら直前カウンタから buffer を復元する。不正入力では `failbit` を設定して engine を変更せず、stream の flags も復元する。

textual representation の `X[0]` は最下位語である。`set_counter` / `counter()` の公開配列はこれと逆順なので注意する。

- `min()` / `max()`：1 語の出力範囲を返す。

`Philox4x32` と `Philox4x64` は 10 ラウンドの標準パラメータを使う別名。

## 時間計算量

$N=\mathtt{WordCount}$、$R=\mathtt{RoundCount}$ とする。

- 既定・整数シードコンストラクタ、`seed(value)`：$O(N)$
- SeedSequence コンストラクタ、`seed(seed_sequence)`：$O(N)$ と `SeedSequence::generate` の計算量
- `set_counter`、`counter`、`key`：$O(N)$
- `index`、`min`、`max`、整数の静的定数の参照：$O(1)$
- `multipliers` / `round_consts`：要素参照は $O(1)$、配列全体のコピーは $O(N)$
- `operator()`：最悪 $O(RN)$、ならし $O(R)$
- `discard`：$O(N+RN)$。完全ブロック数には依存しない
- `operator==`：$O(N)$
- `operator<<`：$O(N)$
- `operator>>`：最悪 $O(RN)$、ブロック境界では $O(N)$

保存領域は $O(N)$。`operator()` と `discard` の最悪追加空間計算量は $O(N)$、それ以外の操作は戻り値を除いて $O(1)$。

stream 入出力の追加領域は $O(N)$。

## 注意点

`set_counter` の配列は上位語が先頭で、内部カウンタの最下位語は末尾要素に対応する。`Constants` の並びは乗数、ラウンド定数、乗数、ラウンド定数の順。統計的乱数生成器であり、暗号用途には使用しない。64bit 語の乗算には GCC/Clang の `unsigned __int128` 拡張を使う。stream 演算子は hosted 環境向けである。
