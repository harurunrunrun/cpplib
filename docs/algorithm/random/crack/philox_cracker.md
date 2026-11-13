---
title: Philox State Cracker (Philox状態復元)
documentation_of: ../../../../src/algorithm/random/crack/philox_cracker.hpp
---

Philox のブロックラウンドを順方向・逆方向に計算する。キーが既知なら、ブロック境界とカウンタが未知の連続出力から、取り得るすべてのブロック内位置（アラインメント）、観測開始カウンタ、観測直後の将来列を直接復元する。整数シードの候補区間を調べる従来の API も提供する。

## ブロックの生成と逆変換

~~~cpp
philox_generate_block<Engine>(counter, key)
philox_invert_block<Engine>(output, key)
~~~

- `philox_generate_block`: 公開順の `counter` と `key` から 1 ブロックを返す。
- `philox_invert_block`: 1 ブロックと `key` から、そのブロックを生成したカウンタを返す。

カウンタの語順は `PhiloxEngine::set_counter` と同じく上位語が先頭である。出力ブロックは `operator()` が返す順である。逆変換は、奇数の乗数の $2^w$ を法とする乗法逆元を Newton 法で求め、ラウンドを逆順に適用する。語が $[0,2^w)$ を外れる場合、および乗数が偶数で逆変換できない場合は `std::invalid_argument` を送出する。

## PhiloxKnownKeyCracker

~~~cpp
PhiloxKnownKeyCracker<Engine>(
    key,
    first_counter,
    first_index
)
~~~

既知キー、最初のブロックカウンタ、そのブロック内で最初に返す語の添字から将来列を生成する。

- `engine_type` / `result_type` / `counter_type` / `key_type` / `block_type`: 対応する型。
- デフォルトコンストラクタ: 零キー・零カウンタで、次の呼出しからブロックを生成する。
- 3 引数コンストラクタ: `first_counter` のブロックを用意し、`first_index` を次の語にする。
- `next()` / `operator()()`: 次の語を返す。
- `key()`: 既知キーへの const 参照を返す。
- `next_block_counter()`: 現在のバッファの次に生成するブロックカウンタへの const 参照を返す。
- `buffer_index()`: 現在のバッファで次に返す添字を返す。`word_count` と等しいときは次の呼出しでブロックを生成する。

`first_index >= word_count` なら `std::out_of_range` を、カウンタ・キーの語幅違反なら `std::invalid_argument` を送出する。

## PhiloxKnownKeyCandidate

~~~cpp
PhiloxKnownKeyCandidate<Engine>
~~~

1 個の観測開始状態を表す。

- `counter_type` / `predictor_type`: 対応する型。
- `first_counter`: 観測先頭語を含むブロックのカウンタ。
- `first_index`: そのブロック内の観測開始添字。
- `predictor`: 全観測を消費した直後の cracker。

## PhiloxKnownKeyRecovery

~~~cpp
PhiloxKnownKeyRecovery<Engine>
~~~

- `engine_type` / `result_type` / `candidate_type`: 対応する型。
- `maximum_candidates == word_count`: アラインメント候補数の上限。
- `minimum_observation_count == 3 * word_count - 1`: 任意の開始添字で完全な連続 2 ブロックを含むために必要な観測数。
- `candidates` / `candidate_count`: 一致した全候補と有効要素数。
- `unique()`: 候補が 1 個かを返す。
- `next()`: 一意候補の次出力を返す。一意でなければ `std::logic_error` を送出する。

## 既知キーの連続列復元

~~~cpp
recover_philox_known_key_stream<Engine>(outputs, key)
~~~

$N$ を `word_count` として、$3N-1$ 語以上の連続出力と既知キーを受け取る（不足すると `std::invalid_argument`）。$N$ 個の開始添字をすべて調べ、各候補で完全な 2 ブロックを `philox_invert_block` によりカウンタへ戻す。カウンタが連続し、全観測を再生成できる候補だけを返す。カウンタ値やシード候補範囲は走査しない。

情報量の少ない構成では複数のアラインメントが同じ観測を説明し得るため、API は最初の候補だけでなく全候補を返す。

## 整数シード既知の標準構成

~~~cpp
recover_philox_integer_seeded_stream<Engine>(outputs, seed)
~~~

`PhiloxEngine::seed(value)` と同じく、キーの先頭語を $\mathrm{seed} \bmod 2^w$（$w$ は `word_size`）、残りを 0 として `recover_philox_known_key_stream` を呼ぶ。`Philox4x32` と `Philox4x64` の任意のカウンタ・ブロック開始位置を直接扱える。返り値と必要観測数は `PhiloxKnownKeyRecovery` と同じ。

## 1 ラウンド構成の未知キーの直接復元

~~~cpp
recover_one_round_philox_key<Engine>(counter, output)
recover_one_round_philox_integer_seed<Engine>(counter, output)
~~~

`Engine::round_count == 1` の構成で使用する。既知カウンタと 1 ブロックから、積の下位語で整合性を検査し、積の上位語とカウンタ語の XOR をとって各キー語を直接求める。一致しなければ `std::nullopt`。

整数シード版はキーの第 2 語以降が 0 であることも検査し、`PhiloxIntegerSeedClass<Engine>` を返す。

## PhiloxIntegerSeedClass

- `result_type`: `Engine::result_type`。
- `masked_seed`: キー先頭語に保存される $w$ ビット。
- `contains(seed)`: $\mathrm{seed} \bmod 2^w$ が等しいかを返す。
- `canonical_seed()`: `masked_seed` を代表シードとして返す。

`result_type` が語幅より広ければ、異なる数値シードが同じクラスに属する。

## 従来の有限候補シード探索

~~~cpp
recover_philox_seed_candidates<Engine>(
    outputs,
    discard_count,
    seed_first,
    candidate_count
)
recover_philox_seed_in_range<Engine>(
    outputs,
    discard_count,
    seed_first,
    seed_last
)
~~~

`PhiloxSeedRecoveryStatus` の `none`, `unique`, `ambiguous` は、指定した数値シード候補中の一致数を表す。

`PhiloxSeedRecovery<Engine>` は `result_type`, `status`, `seed`, `predictor` を持つ。`unique()` は一致数が 1 かを返し、`recovered_seed()` は一意シードまたは `std::nullopt`、`next()` は一意な場合の次出力を返す。一意でないときの `next()` は `std::logic_error` を送出する。

候補数形式は `seed_first + offset`（`0 <= offset < candidate_count`）を調べる。候補の終端が `result_type` の範囲を越える場合は `std::invalid_argument` を送出する。範囲形式は半開区間 [`seed_first`, `seed_last`) を調べ、逆順なら `std::invalid_argument` を送出する。観測列が空の場合も `std::invalid_argument` を送出する。

## 時間計算量

$N$ を `word_count`、$R$ を `round_count`、$K$ を観測語数、$D$ を従来 API のシード候補数とする。

- `philox_generate_block`, `philox_invert_block`: $O(RN)$
- `PhiloxKnownKeyCracker` の 3 引数コンストラクタ: $O(RN)$
- デフォルトコンストラクタ、`key`, `next_block_counter`, `buffer_index`: $O(1)$
- `next` / `operator()`: 最悪 $O(RN)$、償却 $O(R)$
- `PhiloxKnownKeyRecovery::unique`, 一意な `next`: $O(1)$（`next` 自体の生成計算量は上記と同じ）
- `recover_philox_known_key_stream`, `recover_philox_integer_seeded_stream`: $O(RN(N+K))$ 時間、返り値を含めて $O(N^2)$ 領域
- `recover_one_round_philox_key`, `recover_one_round_philox_integer_seed`: $O(N)$ 時間・$O(N)$ 戻り値領域
- `PhiloxIntegerSeedClass` の各操作: $O(1)$
- `recover_philox_seed_candidates`, `recover_philox_seed_in_range`: $O(D(N+R(N+K)))$
- `PhiloxSeedRecovery` の各クエリ: $O(1)$（`next` 自体の生成計算量は上記と同じ）

## 注意点

- Philox ラウンドはキーを固定したカウンタ上の置換である。ラウンドの可逆性は既知キーからカウンタを戻すものであり、10 ラウンドの未知キーを出力だけから高速に得られることを意味しない。
- `Philox4x32` / `Philox4x64` の未知キー復元は既知平文カウンタに対する鍵探索問題である。このヘッダは範囲の総当たりを直接復元とは呼ばない。キーが既知なら標準の 10 ラウンド構成のカウンタ・アラインメント・将来列を直接復元し、未知キーを直接解ける 1 ラウンド構成は別の API とする。
- 既知キー列の復元には加工前の連続出力を使う。
- Philox は統計的乱数生成器であり、暗号用途には使用しない。
- 既知キーの復元、整数シード既知の復元、シード候補探索には `std::span` を受け取るオーバーロードがあり、観測数は実行時に決めてよい（固定長 `std::array` 版も同じヘッダにある）。観測列はコピーせず、シード候補探索は候補ごとの領域を再利用する。

~~~cpp
Philox4x32 generator(7);
std::vector<Philox4x32::result_type> outputs(16);
for(auto& value: outputs) value = generator();
Philox4x32::key_type key{};
key[0] = 7;
auto recovery = recover_philox_known_key_stream<Philox4x32>(
    std::span<const Philox4x32::result_type>(outputs), key
);
~~~
