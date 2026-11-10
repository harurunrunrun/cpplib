---
title: Count Decreasing Triplets (減少三つ組の個数) [TRIPINV]
documentation_of: ../../../../src/algorithm/other/sequence/count_decreasing_triplets.hpp
---

列中で添字 $i<j<k$ かつ $a_i>a_j>a_k$ を満たす三つ組の個数を返す。各要素について、左側にある自分より大きい要素の個数と右側にある自分より小さい要素の個数の積を Fenwick 木で数えて足し合わせる。

## count_decreasing_triplets

```cpp
template<class T, int MAX_SIZE = -1>
long long count_decreasing_triplets(const vector<T>& values);
```

`MAX_SIZE` を省略すると、内部の Fenwick 木に実行時サイズの `std::vector` を使う可変長版になる。指定すると固定長版になる。

```cpp
auto result = count_decreasing_triplets(values);
auto fixed = count_decreasing_triplets<Value, 100000>(values);
```

## 時間計算量

要素数を $N$ として $O(N\log N)$。

## 空間計算量

可変長版は $O(N)$、固定長版は $O(N+\texttt{MAX_SIZE})$。

## 注意点

- `T` は全順序を持たなければならない。
- 等しい要素は厳密な大小関係に含めない。
- 固定長版では相異なる値の個数が `MAX_SIZE` 以下でなければならない。
- 結果は `long long` で表現できなければならない。
