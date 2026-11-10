---
title: Subtract-with-Carry State Cracker (キャリー付き減算法状態復元)
documentation_of: ../../../../src/algorithm/random/crack/subtract_with_carry_cracker.hpp
---

`std::subtract_with_carry_engine` の加工されていない連続出力から、繰り上がりを含む内部状態と将来列を直接復元する。標準の ranlux base については、整数シードの全域を走査せず、標準初期化が作るシード同値類も逆算する。

## SubtractWithCarryCracker

~~~cpp
SubtractWithCarryCracker<UInt, WordSize, ShortLag, LongLag> cracker(outputs);
~~~

`UInt` は 64 bit 以下の符号なし整数、`WordSize` は 1 以上かつ `UInt` のビット数以下、`ShortLag` と `LongLag` は $0<\mathtt{ShortLag}<\mathtt{LongLag}$ を満たす。

- `result_type`: 生成値に使う `UInt`。
- `state_type`: 長さ `LongLag` の状態配列。
- `SubtractWithCarryCracker(outputs)`: 長さ `Count` の `std::array<result_type, Count>` から状態と繰り上がりを復元する。`Count` は `LongLag + 1` 以上でなければならない（`static_assert`）。`std::span<const result_type>` 版もあり、こちらは観測数が不足すると `std::invalid_argument` を送出する。余分な出力もすべて漸化式と照合して消費するため、成功後は `outputs.back()` の直後を指す。語のビット幅違反または途中で漸化式と一致しない場合は `std::invalid_argument` を送出する。
- `next()`: 次の生出力を返し、状態を 1 回進める。
- `operator()()`: `next()` と同じ。
- `discard(count)`: 生出力を返さずに `count` 回進める。
- `carry()`: 次の遷移で使う現在の繰り上がりを返す。
- `state()`: 直近 `LongLag` 語を古い順に返す。
- `min()`: 0 を返す。
- `max()`: `WordSize` ビットがすべて 1 の値を返す。

`Ranlux24BaseCracker` は `SubtractWithCarryCracker<std::uint_fast32_t, 24, 10, 24>`、`Ranlux48BaseCracker` は `SubtractWithCarryCracker<std::uint_fast64_t, 48, 5, 12>` の型別名である。

## StandardSubtractWithCarrySeedClass

~~~cpp
StandardSubtractWithCarrySeedClass<SeedUInt>
~~~

標準の整数シード初期化が内部で使う `linear_congruential_engine<uint_least32_t, 40014, 0, 2147483563>` の初期状態 1 個を、同じ生成列を作る整数シードの同値類として表す。

- `seed_type`: 整数シードの型。
- `lcg_modulus` / `lcg_default_seed`: 標準初期化の定数。
- `lcg_state`: 最初の LCG 呼出し直前の正規化済み状態。
- `contains(seed)`: `seed` がこの同値類に属するかを返す。
- `contains_zero_seed()`: 引数 0 の特別なデフォルトシードが属するかを返す。
- `canonical_seed()`: 同値類の代表値を返す。0 を含む類では 0、それ以外では `lcg_state` を返す。

0 以外の整数シードは LCG の法で剰余を取り、剰余 0 を 1 へ直す。したがって異なる整数シードが同じ同値類に属し得る。

## StandardSubtractWithCarrySeedRecovery

~~~cpp
StandardSubtractWithCarrySeedRecovery<Cracker, SeedUInt>
~~~

- `cracker_type` / `result_type` / `seed_type` / `seed_class_type`: 対応する型。
- `maximum_seed_classes`: 返し得る同値類数の上限 512。4 個以下の初期状態それぞれで、24 bit 構成では LCG 出力の上位ビットの補完候補を最大 128 個調べる。
- `seed_classes` / `seed_class_count`: 観測に一致する全シード同値類と有効要素数。
- `predictor`: 全観測を消費した直後の状態 cracker。
- `unique_seed_class()`: 同値類が 1 個かを返す。
- `canonical_seed()`: 同値類が 1 個なら代表シード、それ以外は `std::nullopt`。
- `contains_seed(seed)`: いずれかの同値類に属するかを返す。
- `next()`: 観測直後の次出力を返す。

`unique_seed_class()` は数値シードそのものが一意という意味ではない。同じ LCG 状態へ正規化されるすべての整数シードを `seed_classes` が表す。

## 標準 ranlux base の直接シード復元

~~~cpp
recover_ranlux24_base_standard_seed(outputs)
recover_ranlux48_base_standard_seed(outputs)
~~~

`std::ranlux24_base` または `std::ranlux48_base` を整数シードで初期化した直後から始まる連続生出力を受け取る。前者は 25 語以上、後者は 13 語以上必要である。

最初の lag 区間を繰り上がりの境界条件から最大 4 状態へ直接逆算し、標準 LCG の最初の出力から乗数 40014 の法 2147483563 における逆元を使って初期 LCG 状態を得る。`ranlux24_base` では 24 bit 語から LCG 出力の上位ビットを最大 128 通り補完し、全初期状態と照合する。`ranlux48_base` では先頭状態語の下位 32 bit が LCG 出力を完全に保持する。整数シード区間は走査しない。

返り値の型別名は `Ranlux24BaseStandardSeedRecovery` と `Ranlux48BaseStandardSeedRecovery` である。観測が有効な漸化式なら、標準シード同値類が 0 個でも `predictor` と `next()` による将来予測は利用できる。観測開始が初期出力であるという前提に一致しない場合、`seed_classes` はそのずれた列を初期列として説明する同値類だけを返す。

## SubtractWithCarrySeedRecoveryStatus

- `none`: 一致するシードがない。
- `unique`: 一致するシードがちょうど 1 個である。
- `ambiguous`: 一致するシードが 2 個以上である。

## SubtractWithCarrySeedRecovery

- `result_type`: `Engine::result_type`。
- `status`: 探索結果を表す `SubtractWithCarrySeedRecoveryStatus`。
- `seed`: 見つかったシード。`status == unique` のときだけ有効。
- `predictor`: 観測列を消費した直後の生成器。`status == unique` のときだけ有効。
- `unique()`: 一致するシードが 1 個だけなら `true` を返す。
- `recovered_seed()`: 一意なシードを `std::optional` で返す。一意でなければ `std::nullopt`。
- `next()`: 一意な `predictor` の次出力を返す。`status != unique` では `std::logic_error` を送出する。

## recover_subtract_with_carry_seed_candidates

~~~cpp
recover_subtract_with_carry_seed_candidates<Engine>(
    outputs, discard_count, seed_first, candidate_count
)
~~~

`seed_first` から連続する `candidate_count` 個、すなわち `seed_first + offset`（`0 <= offset < candidate_count`）を全探索する。各候補で生成器を初期化し、`discard_count` 回進めてから長さ `Count` の `outputs` と照合する。一意な場合の `predictor` は全観測を消費した直後を指す。

`candidate_count == 0` なら `none` を返す。最後の候補が `Engine::result_type` の最大値を越える場合は `std::invalid_argument`。加算で終端を作らないため、最大値そのものも、例えば `seed_first == max` かつ `candidate_count == 1` として安全に探索できる。`Count` は 1 以上で、`Engine::result_type` は `unsigned long long` 以下のビット数を持つ符号なし整数とする。

## recover_subtract_with_carry_seed_in_range

~~~cpp
recover_subtract_with_carry_seed_in_range<Engine>(
    outputs, discard_count, seed_first, seed_last
)
~~~

従来互換の半開区間 `[seed_first, seed_last)` を全探索する。候補数を `seed_last - seed_first` として `recover_subtract_with_carry_seed_candidates` に委譲する。`seed_last < seed_first` では `std::invalid_argument` を送出する。同じ整数型の排他的終端では最大値の次を表現できないため、最大シードを含める場合は候補数 API を使う。

## 時間計算量

$M$ を状態復元へ渡した観測数、$D$ をシード候補数、$K$ をシード探索の観測数、$Q$ を `discard_count` とする。

- `SubtractWithCarryCracker(outputs)`: $O(M)$ 時間、$O(\mathtt{LongLag})$ 保存領域
- `next()` / `operator()()`: $O(1)$
- `discard(count)`: $O(\mathtt{count})$
- `carry()`: $O(1)$
- `state()`: $O(\mathtt{LongLag})$ 時間・戻り値領域
- `min()` / `max()`: $O(1)$
- `unique()` / `recovered_seed()` / `SubtractWithCarrySeedRecovery::next()`: $O(1)$
- `recover_ranlux24_base_standard_seed` / `recover_ranlux48_base_standard_seed`: $O(M+\mathtt{LongLag})$ 時間・$O(\mathtt{LongLag})$ 追加領域。固定パラメータ内では前者が最大 128 個、後者が 1 個の LCG 出力候補を照合する
- `StandardSubtractWithCarrySeedClass` の各操作、`unique_seed_class` / `canonical_seed` / 直接復元結果の `next`: $O(1)$。`contains_seed`: $O(C)$、$C$ は返した同値類数で $C\leq512$
- 公開型別名・列挙子・データメンバの参照: $O(1)$
- `recover_subtract_with_carry_seed_candidates` / `recover_subtract_with_carry_seed_in_range`: $O(D(\mathtt{LongLag}+Q+K))$ 時間、$O(\mathtt{LongLag})$ 追加領域

## 注意点

- 直接シード復元 API は、整数シード初期化直後の第 1 出力から観測する。読み飛ばした後の列でも状態と将来列は `SubtractWithCarryCracker` で復元できるが、初期シード同値類の判定には開始位置の情報が必要である。
- 標準初期化は整数シードを LCG の法で正規化するため、一般に数値シードは一意でない。`seed_classes` と `contains_seed` で同値類全体を扱う。

- 復元には最低 `LongLag + 1` 個の、同じ生成器から得た連続生出力が必要である。追加観測を渡すと誤った列を早期に検出し、復元後の位置もその分だけ進む。
- シード探索で得られる一意性は、指定した候補集合と `discard_count` の中だけでの一意性である。既知の読み飛ばし数を正確に渡すこと。
- `ranlux24` / `ranlux48` は `std::discard_block_engine` による間引き後の列なので直接対象にしない。生の `ranlux24_base` / `ranlux48_base` に使用する。
- 状態復元、標準シードの直接復元、有限シード候補探索には `std::span` を渡せ、観測数は実行時に決めてよい（`std::array` 版も同じヘッダにある）。観測列はコピーしない。

~~~cpp
std::ranlux24_base generator(123);
std::vector<std::ranlux24_base::result_type> outputs(40);
for(auto& value: outputs) value = generator();
auto observations = std::span<const std::ranlux24_base::result_type>(outputs);
Ranlux24BaseCracker cracker(observations);
auto seed_classes = recover_ranlux24_base_standard_seed(observations);
~~~
