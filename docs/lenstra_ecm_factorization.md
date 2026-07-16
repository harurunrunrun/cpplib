---
title: Lenstra ECM Factorization (Lenstra楕円曲線法による素因数分解)
documentation_of: ../src/algorithm/math/lenstra_ecm_factorization.hpp
---

`2^127` 未満の整数から未知の非自明因子を確率的に探索し、また素因数分解する。Suyama の曲線生成、Montgomery 曲線の `x/z` 座標による ladder、prime-power を掛ける Stage 1 と、baby-step/giant-step による Stage 2 を実装している。Pollard 法のラッパーではない。

## `LenstraEcmParameters` (`Lenstra ECMのパラメータ`)

- `stage1_bound`: Stage 1 の smoothness bound `B1`。既定値は `20000`。
- `stage2_bound`: Stage 2 が扱う素数の上限 `B2`。既定値は `400000`。
- `curves`: 1 回の因子探索で試す曲線数。
- `stage2_wheel`: Stage 2 の baby-step/giant-step の幅。
- `gcd_batch_size`: Stage 2 で積をまとめて GCD する個数。
- `primality_rounds`: Miller--Rabin の反復回数。
- `factorization_retries`: 完全分解で因子探索を再試行する回数。
- `seed`: 曲線と Miller--Rabin の疑似乱数 seed。同じ入力とパラメータなら結果は再現可能。

`B1 >= 2`, `B2 >= B1`, `stage2_wheel >= 2`, `gcd_batch_size > 0`, `primality_rounds > 0`, `factorization_retries > 0` が必要である。

## `lenstra_ecm_find_factor(value, parameters)` (`非自明因子の探索`)

`abs(value)` の非自明因子を `std::optional<BigInteger>` で返す。素数、`0`, `1`、または指定曲線数で見つからなかった場合は `std::nullopt` を返す。入力の絶対値が `2^127` 以上なら `std::overflow_error`、パラメータが不正なら `std::invalid_argument` を送出する。

時間計算量は、`M(b)` を `b` bit の剰余乗算時間、`pi(x)` を `x` 以下の素数個数として、1 曲線あたり Stage 1 が `O(pi(B1) log(B1) M(b))`、Stage 2 が概ね `O((B2 / D + pi(B2) + D) M(b))` (`D = stage2_wheel`) である。素因子 `p` が見つかる確率は、`p` 上で選ばれた曲線の位数が `B1`-smooth、または高々 1 個の `(B1,B2]` の素因子を除いて `B1`-smooth になる確率に依存する。失敗は誤答ではなく `nullopt` であり、`curves`, `B1`, `B2` を増やせば成功確率が上がる。

## `factorize_lenstra_ecm(value, parameters)` (`完全素因数分解`)

`abs(value)` を再帰的に分解し、昇順の `(素因数, 指数)` を返す。`value == 0` は `std::invalid_argument`。指定された合計曲線予算を使い切った場合は `std::runtime_error` を送出し、不完全な結果は返さない。`1` の結果は空である。

期待時間計算量は発見対象の最大の「小さい方の素因数」に依存し、各再帰節点で上記因子探索の計算量がかかる。Miller--Rabin が合成数を素数と誤判定する確率は、独立一様な底を仮定すると高々 `4^(-primality_rounds)` である。ECM 自体は Las Vegas 型で、返した因子について必ず割り切れることを確認してから再帰する。

## 注意点

- 内部では外部多倍長ライブラリを使わず、`__uint128_t` 上の 2-limb Montgomery 乗算を使う。従って現在の上限は符号を除いて 127 bit であり、`10^38` 級を含む。
- 大きさが同程度の 60--63 bit 素因数を持つ半素数では、小さい `B1` や少ない曲線数では失敗しやすい。用途に応じてパラメータを増やす。
- `seed` は暗号学的乱数ではない。このライブラリは競技プログラミング向けであり、暗号用途を想定しない。

## 検証

独立 generator が既知の素因数から境界・冪・ランダム半素数を作り、checker が正規化された完全分解を照合する。直接テストでは Montgomery 乗算を加算ベースの独立実装と比較し、例外、負数、素数、乱数ケースを検査する。
