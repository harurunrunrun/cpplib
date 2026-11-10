---
title: Intersection of F2 Vector Spaces (二元体ベクトル空間の共通部分) [intersection_of_f2_vector_spaces]
documentation_of: ../../../../../src/algorithm/math/linear_algebra/gf2/intersection_of_f2_vector_spaces.hpp
---

符号なし整数を $\mathbb{F}_2$ 上の bit ベクトルとみなし、2 つの生成系が張る部分空間の共通部分を求める。一方の空間で消去した剰余をもう一方の基底について掃き出し、商空間で 0 になる組合せを取り出す。

## `intersection_of_f2_vector_spaces`

```cpp
template<std::unsigned_integral UInt>
std::vector<UInt> intersection_of_f2_vector_spaces(
    const std::vector<UInt>& first, const std::vector<UInt>& second);
```

`first` と `second` がそれぞれ張る空間の共通部分の基底を返す。入力は一次独立でなくてもよい。`UInt` は符号なし整数型である。

戻り値は各ピボット bit の高い順に並ぶ被約基底である。同じ部分空間に対する戻り値は、入力した生成系の選び方や順序によらない。共通部分が零空間なら空の `std::vector` を返す。

## 時間計算量・空間計算量

$n$ を `first.size()`、$m$ を `second.size()`、$W$ を `std::numeric_limits<UInt>::digits` とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `intersection_of_f2_vector_spaces` | $O((n+m)W+W^2)$ | $O(W)$ |

## 注意点

- 例外は送出しない。基底は固定長の配列（長さ $W$）で保持するため、追加の動的確保は戻り値のみである。
