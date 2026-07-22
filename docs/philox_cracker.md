---
title: Philox State Cracker (Philox状態復元)
documentation_of: ../src/algorithm/random/crack/philox_cracker.hpp
---

Philoxのblock roundを順方向・逆方向に計算する。
keyが既知なら、block境界とcounterが未知の連続出力から、取り得る全alignment、
観測開始counter、観測直後の将来列を直接復元する。
整数seed候補区間を調べる従来APIも保持する。

## blockの生成と逆変換

~~~cpp
philox_generate_block<Engine>(counter, key)
philox_invert_block<Engine>(output, key)
~~~

- `philox_generate_block`：公開順の `counter` と `key` から1blockを返す。
- `philox_invert_block`：1blockと `key` から、そのblockを生成したcounterを返す。

counterの語順は `PhiloxEngine::set_counter` と同じく上位語が先頭。
出力blockは `operator()` が返す順である。
逆変換は奇数multiplierのmodulo $2^w$ における乗法逆元をNewton iterationで求め、
roundを逆順に適用する。
語が $[0,2^w)$ を外れる場合は `std::invalid_argument`。
偶数multiplierは逆変換できないため `std::invalid_argument`。

## PhiloxKnownKeyCracker

~~~cpp
PhiloxKnownKeyCracker<Engine>(
    key,
    first_counter,
    first_index
)
~~~

既知key、最初のblock counter、そのblock内で最初に返す語の添字から
将来列を生成する。

- `engine_type` / `result_type` / `counter_type` /
  `key_type` / `block_type`：対応する型。
- default constructor：zero key・zero counterで、次の呼出しからblockを生成する。
- 3引数constructor：`first_counter` のblockを用意し、
  `first_index` を次の語にする。
- `next()` / `operator()()`：次の語を返す。
- `key()`：既知keyへのconst参照を返す。
- `next_block_counter()`：現在bufferの次に生成するblock counterへのconst参照を返す。
- `buffer_index()`：現在bufferで次に返す添字を返す。
  `word_count` と等しいときは次の呼出しでblockを生成する。

`first_index >= word_count` は `std::out_of_range`。
counter・keyの語幅違反は `std::invalid_argument`。

## PhiloxKnownKeyCandidate

~~~cpp
PhiloxKnownKeyCandidate<Engine>
~~~

1個の観測開始状態を表す。

- `counter_type` / `predictor_type`：対応する型。
- `first_counter`：観測先頭語を含むblockのcounter。
- `first_index`：そのblock内の観測開始添字。
- `predictor`：全観測を消費した直後のcracker。

## PhiloxKnownKeyRecovery

~~~cpp
PhiloxKnownKeyRecovery<Engine>
~~~

- `engine_type` / `result_type` / `candidate_type`：対応する型。
- `maximum_candidates == word_count`：alignment候補数の上限。
- `minimum_observation_count == 3 * word_count - 1`：
  任意の開始添字で完全な連続2blockを含むために必要な観測数。
- `candidates` / `candidate_count`：一致した全候補と有効要素数。
- `unique()`：候補が1個かを返す。
- `next()`：一意候補の次出力を返す。一意でなければ `std::logic_error`。

## 既知keyの連続列復元

~~~cpp
recover_philox_known_key_stream<Engine>(outputs, key)
~~~

$3N-1$ 語以上の連続出力と既知keyを受け取る。
$N=mathtt{word\_count}$ 個の開始添字をすべて調べ、各候補で完全な2blockを
`philox_invert_block` によりcounterへ戻す。
counterが連続し、全観測を再生成できる候補だけを返す。
counter値やseed候補範囲は走査しない。

情報量の少ない構成では複数alignmentが同じ観測を説明し得るため、
APIは最初の候補だけでなく全候補を返す。

## 整数seed既知の標準構成

~~~cpp
recover_philox_integer_seeded_stream<Engine>(outputs, seed)
~~~

`PhiloxEngine::seed(value)` と同じく、key先頭語を
`seed mod 2^word_size`、残りを0として
`recover_philox_known_key_stream` を呼ぶ。
`Philox4x32` と `Philox4x64` の任意のcounter・block開始位置を直接扱える。
返り値と必要観測数は `PhiloxKnownKeyRecovery` と同じ。

## 1-roundの未知key直接復元

~~~cpp
recover_one_round_philox_key<Engine>(counter, output)
recover_one_round_philox_integer_seed<Engine>(counter, output)
~~~

`Engine::round_count == 1` の構成で使用する。
既知counterと1blockから、low productで整合性を検査し、
high product・counter語をxorして各key語を直接求める。
一致しなければ `std::nullopt`。

整数seed版はkeyの第2語以降が0であることも検査し、
`PhiloxIntegerSeedClass<Engine>` を返す。

## PhiloxIntegerSeedClass

- `result_type`：`Engine::result_type`。
- `masked_seed`：key先頭語に保存される $w$ bit。
- `contains(seed)`：`seed mod 2^w` が同じかを返す。
- `canonical_seed()`：`masked_seed` を代表seedとして返す。

`result_type` がword幅より広ければ、異なる数値seedが同じclassに属する。

## 従来の有限候補seed探索

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

`PhiloxSeedRecoveryStatus` の `none`, `unique`, `ambiguous` は、
指定した数値seed候補中の一致数を表す。

`PhiloxSeedRecovery<Engine>` は
`result_type`, `status`, `seed`, `predictor` を持つ。
`unique()` は一致数が1かを返し、`recovered_seed()` は一意seedまたは
`std::nullopt`、`next()` は一意な場合の次出力を返す。
一意でない `next()` は `std::logic_error`。

候補数形式は `seed_first + offset`
（$0\leq offset<candidate\_count$）を調べる。
候補終端がresult typeを越える場合は `std::invalid_argument`。
range形式は半開区間 $[seed\_first,seed\_last)$ で、逆順なら
`std::invalid_argument`。

## 時間計算量

$N=mathtt{word\_count}$、$R=mathtt{round\_count}$、
$K$ を観測語数、$D$ を従来APIのseed候補数とする。

- `philox_generate_block`, `philox_invert_block`: $O(RN)$
- `PhiloxKnownKeyCracker` の3引数constructor: $O(RN)$
- default constructor、`key`, `next_block_counter`, `buffer_index`: $O(1)$
- `next` / `operator()`: 最悪 $O(RN)$、ならし $O(R)$
- `PhiloxKnownKeyRecovery::unique`, 一意な `next`: $O(1)$
  （`next` 自体の生成計算量は上記と同じ）
- `recover_philox_known_key_stream`,
  `recover_philox_integer_seeded_stream`:
  $O(RN(N+K))$ 時間、返り値を含めて $O(N^2)$ 領域
- `recover_one_round_philox_key`,
  `recover_one_round_philox_integer_seed`: $O(N)$ 時間・$O(N)$ 戻り値領域
- `PhiloxIntegerSeedClass` の各操作: $O(1)$
- `recover_philox_seed_candidates`,
  `recover_philox_seed_in_range`: $O(D(N+R(N+K)))$
- `PhiloxSeedRecovery` の各query: $O(1)$
  （`next` 自体の生成計算量は上記と同じ）

## 注意点

- Philox roundはkeyを固定したcounter上の置換である。
  roundの可逆性は既知keyからcounterを戻すものであり、
  10-roundの未知keyを出力だけから高速に得られることを意味しない。
- `Philox4x32` / `Philox4x64` の未知key復元は既知平文counterに対する
  鍵探索問題である。このheaderは範囲総当たりを直接復元と呼ばず、
  key既知なら標準10-round構成のcounter・alignment・将来列を直接復元し、
  key未知を直接解ける1-round構成を別APIにする。
- 既知key列の復元には加工前の連続出力を使う。
- Philoxは統計的乱数生成器であり、暗号用途には使用しない。
