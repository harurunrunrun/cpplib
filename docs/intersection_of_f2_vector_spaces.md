---
title: Intersection of F2 Vector Spaces (二元体ベクトル空間の共通部分)
documentation_of: ../src/algorithm/math/intersection_of_f2_vector_spaces.hpp
---

unsigned整数を$\mathbb{F}_2$上のbit vectorとみなし、2つの生成系が張る部分空間の共通部分を求める。
一方の空間で消去した剰余をもう一方の基底について掃き出し、商空間で0になる組合せを取り出す。

## `intersection_of_f2_vector_spaces`

```cpp
std::vector<UInt> basis = intersection_of_f2_vector_spaces(first, second);
```

`first`と`second`がそれぞれ張る空間の共通部分の基底を返す。
入力は線形独立でなくてもよい。`UInt`はunsigned整数型とする。

戻り値は各pivot bitが高い順に並ぶ被約基底である。
同じ部分空間に対する戻り値は、入力した生成系の選び方や順序によらない。
両空間の共通部分が零空間なら空vectorを返す。

## API別の時間計算量・空間計算量

$n=\texttt{first.size()}$、$m=\texttt{second.size()}$、
$W=\texttt{numeric_limits<UInt>::digits}$とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `intersection_of_f2_vector_spaces` | $O((n+m)W+W^2)$ | $O(W)$ |

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
