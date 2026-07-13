---
title: Philox Seed Cracker (Philoxシード候補探索)
documentation_of: ../src/algorithm/random/philox_cracker.hpp
---

Philoxの整数seedを有限候補区間から全探索し、観測列の次を予測する。

## PhiloxSeedRecoveryStatus

<code>none</code> は一致なし、<code>unique</code> は一致が1個、
<code>ambiguous</code> は一致が複数あることを表す。

## recover_philox_seed_in_range

~~~cpp
recover_philox_seed_in_range<Engine>(
    consecutive_outputs,
    discard_count,
    seed_first,
    seed_last
)
~~~

各seed候補で <code>Engine</code> を構築し、<code>discard_count</code> 回進めてから
全観測値を照合する。seed区間は半開区間。逆順の区間では
<code>std::invalid_argument</code>。観測配列の要素数 <code>Count</code> は
1以上でなければならない。

## PhiloxSeedRecovery

- <code>result_type</code>：<code>Engine::result_type</code>。
- <code>status</code>：探索結果を表す <code>PhiloxSeedRecoveryStatus</code>。
- <code>seed</code>：見つかったseed。<code>status == unique</code> のときだけ有効。
- <code>predictor</code>：観測列を消費した直後の生成器。<code>status == unique</code> のときだけ有効。
- <code>unique()</code>：一致seedが1個だけならtrue。
- <code>recovered_seed()</code>：一意seed、または <code>nullopt</code>。
- <code>next()</code>：一意な場合に観測直後の次出力を返す。それ以外では <code>std::logic_error</code>。

## 時間計算量

候補seed数を $D$、観測語数を $K$、<code>Engine</code> の語数とround数を $N,R$ とする。

- <code>recover_philox_seed_in_range</code>：$O(D(N+R(N+K)))$
- <code>unique</code> / <code>recovered_seed</code>、公開データメンバと型別名の参照：$O(1)$
- <code>PhiloxSeedRecovery::next</code>：最悪 $O(RN)$、ならし $O(R)$

結果の保存領域と探索中の追加空間計算量は $O(N)$。

## 注意点

未知keyのPhiloxを少数出力から一般に複製する関数ではない。
32bit全域でも $2^{32}$ 候補、64bit全域では $2^{64}$ 候補となるため、
時刻seedなど現実的な狭い候補区間に限定する。<code>SeedSequence</code> が作る
複数語keyの元入力はこのAPIの対象外。出力は加工前の連続語で、観測開始までの
呼出回数 <code>discard_count</code> が既知でなければならない。
