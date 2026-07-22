---
title: Pfaffian (パフィアン) [pfaffian_of_matrix]
documentation_of: ../src/algorithm/math/linear_algebra/pfaffian.hpp
---

偶数次交代行列$A$のPfaffianを、対称な行・列交換を伴うGaussian eliminationで求める。
$\operatorname{Pf}(A)^2=\det(A)$を満たす符号付きの値を直接返す。

## `pfaffian(vector)`

```cpp
T value = pfaffian(matrix);
```

`std::vector<std::vector<T>>`を値渡しで受け取る。入力は偶数次の正方行列で、
対角成分が0、`a[j][i] == -a[i][j]`の交代行列でなければならない。
形または交代性が不正なら`std::invalid_argument`を送出する。$0\times0$行列の
Pfaffianは1。

`T`は体であり、0判定、四則演算、単項minus、`T(1)`を提供する必要がある。

## `pfaffian(Matrix)`

```cpp
T value = pfaffian(fixed_capacity_matrix);
```

`Matrix<T, MAX_ROW, MAX_COL>`用overload。実際の行数・列数を動的vectorへコピーし、
同じ検査と消去を行う。非正方形なら`std::invalid_argument`。

## API別の時間計算量・空間計算量

`N`を行列の次数とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| vector版 | $O(N^3)$ | $O(N^2)$ |
| `Matrix`版 | $O(N^3)$ | $O(N^2)$ |

各2行・2列のpivotごとに逆元を1回だけ計算する。入力は値渡しまたはcopyされるため、
呼び出し元の行列は変更しない。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
