---
title: GCD / LCM Convolution (GCD・LCM畳み込み)
documentation_of: ../src/algorithm/fft/divisor_convolution.hpp
---

正整数上のGCD畳み込みとLCM畳み込み。
vectorの添字 `i` は整数 `i+1` に対応する。

```cpp
auto gcd_result = gcd_convolution(a, b);
auto lcm_result = lcm_convolution(a, b);
```

入力長を $N$ とすると、返り値は次を満たす。

$$
g_k=\sum_{\gcd(i,j)=k}a_i b_j,
\qquad
l_k=\sum_{\operatorname{lcm}(i,j)=k}a_i b_j.
$$

LCMが $N$ を超える組は返り値に含まれない。
値型はデフォルト構築、加減算、乗算を使える必要がある。

## API別の時間計算量・空間計算量

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `gcd_convolution(a, b)` | $O(N\log(N+1))$ | $O(N)$ |
| `lcm_convolution(a, b)` | $O(N\log(N+1))$ | $O(N)$ |

空vectorも受け付け、空vectorを返す。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
