---
title: Hafnian (ハフニアン)
documentation_of: ../src/algorithm/math/hafnian.hpp
---

偶数次の対称行列 $A$ に対して、完全マッチングに対応する積の総和

$$
\operatorname{haf}(A)=\sum_M\prod_{\{i,j\}\in M}A_{i,j}
$$

を求める。ここで $M$ は頂点集合全体の完全マッチングを走る。対角成分は参照しない。

## `hafnian(vector)`

```cpp
T answer = hafnian(matrix);
```

`std::vector<std::vector<T>>` で与えた対称行列のハフニアンを返す。行列が正方行列でない場合、次数が奇数の場合、または対称でない場合は `std::invalid_argument` を送出する。$0\times0$ 行列に対しては `T(1)` を返す。

要素型 `T` には零元・単位元、加減算、乗算、等値比較が必要である。

## `hafnian(Matrix)`

```cpp
T answer = hafnian(fixed_capacity_matrix);
```

`Matrix<T, MAX_ROW, MAX_COL>` 用のオーバーロード。実際の行数と列数が一致しない場合は `std::invalid_argument` を送出する。

## 計算量

$N$ を行列の次数とする。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `hafnian(vector)` | $O(N^2 2^{N/2})$ | $O(N2^{N/2})$ |
| `hafnian(Matrix)` | $O(N^2 2^{N/2})$ | $O(N2^{N/2}+N^2)$ |

固定した頂点対を含む一閉路の重みを bit DP で求め、集合べき級数の exponential を subset convolution で計算する。
