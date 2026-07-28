---
title: Dirichlet Convolution (Dirichlet畳み込み)
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/dirichlet_convolution.hpp
---

数論的関数のDirichlet畳み込み
$(f*g)(n)=\sum_{d\mid n}f(d)g(n/d)$
をprefix table上で計算する。

## `dirichlet_convolution(first, second)`

両vectorは添字1から $N$ までを使い、添字0を無視する。
返却vectorの添字 $n$ には
$\sum_{d\mid n}\mathtt{first}[d]\mathtt{second}[n/d]$ を格納する。

## `dirichlet_inverse(values)`

`values[1]` が可逆な関数のDirichlet逆を返す。返却値を `inverse` とすると、
`dirichlet_convolution(values, inverse)` は添字1だけが1で、ほかが0となる。
既知の `inverse[q]` から、その寄与を全ての倍数 $dq$ へ伝播して求める。

## 時間計算量

- `dirichlet_convolution`: $O(N\log N)$ 時間、$O(N)$ 空間。
- `dirichlet_inverse`: $O(N\log N)$ 時間、$O(N)$ 空間。

どちらも実行する積和の回数は
$\sum_{q=1}^{N}\lfloor N/q\rfloor=O(N\log N)$ である。

## 注意点

入力vectorの添字0は参照しない。`dirichlet_convolution` の2入力は同じ正の長さで
なければならない。`dirichlet_inverse` は添字1を含む長さでなければならない。
違反時は `std::invalid_argument` を送出する。

`Value` はデフォルト値を加法単位元、`Value{1}` を乗法単位元として扱えなければ
ならず、加算・乗算・除算・単項マイナスを提供する必要がある。
`values[1]` は `Value` 上で可逆でなければならない。この代数的前提は実行時には
検査しない。
