---
title: Convolution (畳み込み)
documentation_of: ../src/algorithm/fft/convolution.hpp
---

NTT による畳み込み。

`arbitrary_mod_convolution` は 3 つの NTT-friendly prime で畳み込んだ結果を CRT で戻す。

## 関数

- `number_theoretic_transform<MOD>(a, inverse = false)`
    - NTT と逆 NTT。
- `convolution_ntt<MOD>(a, b)`
    - `MOD` が NTT 可能な場合の畳み込み。
- `arbitrary_mod_convolution<MOD>(a, b)`
    - 任意の `Modint<MOD>` に対する畳み込み。

## 計算量の概要

- $O(n \log n)$

## API別の時間計算量・空間計算量

$A=|a|$, $B=|b|$、$Z$ を $A+B-1$ 以上の最小の2冪とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `number_theoretic_transform(a,inverse)` | $O(N\log N)$ | 追加 $O(1)$ |
| `convolution_ntt(a,b)` | $O(Z\log Z)$ | $O(Z)$ |
| `arbitrary_mod_convolution(a,b)` | $O(Z\log Z)$ | $O(Z)$ |

畳み込みは $A+B-1$ 要素を列挙して返す。既知のNTT-friendly法以外では3回のNTTとCRTを行うが、漸近計算量は同じ。

`MOD` の原始根が既知でない場合、最初の変換時だけ `MOD-1` の試し割りと原始根探索を行う。一度得た根はtemplate実体ごとのstatic値として再利用され、2回目以降の前処理は不要。
## 注意点

- `number_theoretic_transform<MOD>(a,inverse)` はvectorをin-placeで変換する。
  長さは正の2冪で、その長さが `MOD-1` を割り切る必要がある。
  `inverse=true` は逆変換と長さの逆元による正規化を行う。
- `convolution_ntt<MOD>(a,b)` は
  $c_k=\sum_{i+j=k}a_i b_j$ を返す。どちらかが空なら空。
  必要な変換長が `MOD-1` を割り切らなければ例外。
- `arbitrary_mod_convolution<MOD>(a,b)` は同じ畳み込みを `Modint<MOD>` で返す。
  既知NTT法では直接、それ以外では3法の結果をCRTで `MOD` へ戻す。
- `MOD` は変換に適した素数であることを前提とし、素数性は検査しない。
