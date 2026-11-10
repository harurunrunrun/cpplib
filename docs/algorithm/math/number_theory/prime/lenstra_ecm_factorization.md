---
title: Lenstra ECM Factorization (Lenstra楕円曲線法による素因数分解)
documentation_of: ../../../../../src/algorithm/math/number_theory/prime/lenstra_ecm_factorization.hpp
---

絶対値が $2^{127}$ 未満の整数について、楕円曲線法（ECM）で未知の非自明な因子を確率的に探索し、また素因数分解する。Suyama の曲線生成、Montgomery 曲線の $X/Z$ 座標による Montgomery ladder、素数冪を掛ける Stage 1、baby-step giant-step による Stage 2 を実装している。Pollard 法のラッパーではない。入出力の整数は `BigInteger` で表す。

## `LenstraEcmParameters`（Lenstra ECM のパラメータ）

- `stage1_bound`: Stage 1 の滑らかさの上限 $B_1$。デフォルトは $20000$。
- `stage2_bound`: Stage 2 が扱う素数の上限 $B_2$。デフォルトは $400000$。
- `curves`: 1 回の因子探索で試す曲線の数。デフォルトは $48$。
- `stage2_wheel`: Stage 2 の baby-step giant-step の幅 $D$。デフォルトは $210$。
- `gcd_batch_size`: Stage 2 で積をまとめて GCD をとる個数。デフォルトは $128$。
- `primality_rounds`: Miller–Rabin の反復回数。デフォルトは $24$。
- `factorization_retries`: 完全分解で因子探索を再試行する回数。デフォルトは $8$。
- `seed`: 曲線の選択と Miller–Rabin に使う疑似乱数のシード。同じ入力とパラメータなら結果は再現できる。

$B_1 \ge 2$、$B_2 \ge B_1$、`stage2_wheel >= 2`、`gcd_batch_size > 0`、`primality_rounds > 0`、`factorization_retries > 0` が必要である。

## `lenstra_ecm_find_factor(value, parameters)`（非自明な因子の探索）

$\lvert \mathrm{value} \rvert$ の非自明な因子を `std::optional<BigInteger>` で返す。素数、$0$、$1$ の場合や、指定した数の曲線で見つからなかった場合は `std::nullopt` を返す。入力の絶対値が $2^{127}$ 以上なら `std::overflow_error`、パラメータが不正なら `std::invalid_argument` を送出する。

時間計算量は、$M(b)$ を $b$ bit の剰余乗算の時間、$\pi(x)$ を $x$ 以下の素数の個数として、1 曲線あたり Stage 1 が $O(\pi(B_1) \log(B_1) M(b))$、Stage 2 がおおよそ $O((B_2/D + \pi(B_2) + D) M(b))$ である。素因数 $p$ が見つかる確率は、$p$ 上で選ばれた曲線の位数が $B_1$-smooth になるか、$(B_1, B_2]$ の素因数を高々 1 個除いて $B_1$-smooth になる確率に依存する。失敗しても誤答にはならず `std::nullopt` が返る。`curves`、$B_1$、$B_2$ を増やせば成功確率が上がる。

## `factorize_lenstra_ecm(value, parameters)`（完全な素因数分解）

$\lvert \mathrm{value} \rvert$ を再帰的に分解し、`(素因数, 指数)` の組を素因数の昇順に返す。`value == 0` では `std::invalid_argument` を送出する。指定された曲線の予算を使い切った場合は `std::runtime_error` を送出し、不完全な結果は返さない。$1$ の結果は空である。

期待時間計算量は、見つける必要がある素因数（各合成数の小さい方の素因数）の最大値に依存し、再帰の各節点で上記の因子探索の計算量がかかる。Miller–Rabin が合成数を素数と誤判定する確率は、底が独立かつ一様と仮定すると高々 $4^{-\mathrm{rounds}}$（$\mathrm{rounds}$ は `primality_rounds`）である。ECM 自体は Las Vegas 型で、返す因子が実際に割り切ることを確認してから再帰する。

## 注意点

- 外部の多倍長ライブラリは使わず、`__uint128_t` 上の 2 語の Montgomery 乗算を使う。そのため現在の上限は符号を除いて 127 bit で、$10^{38}$ 程度の値を含む。
- 同程度の大きさの 60〜63 bit の素因数 2 つからなる半素数は、小さい $B_1$ や少ない曲線数では失敗しやすい。用途に応じてパラメータを増やす。
- `seed` は暗号学的な乱数ではない。競技プログラミング向けであり、暗号用途は想定しない。

## 検証

独立な生成器が既知の素因数から境界値・冪・ランダムな半素数を作り、チェッカーが正規化した完全分解と照合する。直接のテストでは Montgomery 乗算を加算による独立実装と比較し、例外・負数・素数・ランダムなケースを検査する。
