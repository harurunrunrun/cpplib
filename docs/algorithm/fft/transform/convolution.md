---
title: Convolution (畳み込み) [convolution_mod] [convolution_mod_1000000007] [convolution_mod_large] [mul_mod2n_convolution] [mul_modp_convolution]
documentation_of: ../../../../src/algorithm/fft/transform/convolution.hpp
---

NTT による畳み込みである。`arbitrary_mod_convolution` は、3 つの NTT に適した素数（$754974721, 167772161, 469762049$）で畳み込んだ結果を CRT で復元することで、任意の法に対応する。

## 関数

- `void number_theoretic_transform<MOD>(std::vector<Modint<MOD>>& a, bool inverse = false)`
    - NTT とその逆変換。
- `std::vector<Modint<MOD>> convolution_ntt<MOD>(std::vector<Modint<MOD>> a, std::vector<Modint<MOD>> b)`
    - `MOD` で NTT が可能な場合の畳み込み。
- `std::vector<Modint<MOD>> arbitrary_mod_convolution<MOD>(const std::vector<Modint<MOD>>& a, const std::vector<Modint<MOD>>& b)`
    - 任意の `Modint<MOD>` に対する畳み込み。

## 計算量

$A$ を `a` の長さ、$B$ を `b` の長さ、$Z$ を $A + B - 1$ 以上の最小の 2 の冪とする。`number_theoretic_transform` では $N$ を入力長とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `number_theoretic_transform(a, inverse)` | $O(N\log N)$ | 追加 $O(1)$ |
| `convolution_ntt(a, b)` | $O(Z\log Z)$ | $O(Z)$ |
| `arbitrary_mod_convolution(a, b)` | $O(Z\log Z)$ | $O(Z)$ |

`arbitrary_mod_convolution` は、`MOD` が上記 3 素数または $998244353$ でなければ 3 回の畳み込みと CRT を行うが、漸近計算量は同じである。

`MOD` の原始根が既知（$2, 167772161, 469762049, 754974721, 998244353$）でない場合、最初の変換時だけ `MOD - 1` の試し割りと原始根の探索を $O(\sqrt{\texttt{MOD}})$ 程度で行う。得た原始根はテンプレートの実体ごとに `static` 変数として保持し、2 回目以降は再計算しない。

## 注意点

- `number_theoretic_transform<MOD>(a, inverse)` は `a` をインプレースで変換する。長さは正の 2 の冪で、かつ `MOD - 1` を割り切る必要があり、満たさなければ `std::runtime_error` を送出する。`inverse == true` なら逆変換の後に長さの逆元を掛けて正規化する。
- `convolution_ntt<MOD>(a, b)` は $c_k = \sum_{i+j=k} a_i b_j$（長さ $A + B - 1$）を返す。どちらかが空なら空列を返す。必要な変換長が `MOD - 1` を割り切らなければ `std::runtime_error` を送出する。
- `arbitrary_mod_convolution<MOD>(a, b)` は同じ畳み込みを `Modint<MOD>` で返す。`MOD` が上記の NTT 向きの素数なら直接 `convolution_ntt` を呼び、それ以外では 3 つの法での結果を CRT で `MOD` に戻す。3 法の場合の変換長の上限は $167772161 - 1$ を割り切る $2^{25}$ である。
- `MOD` は変換に適した素数であることを前提とし、素数性は検査しない。
