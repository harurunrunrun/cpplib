---
title: Inverse Matrix (Mod 2) (二元体上の逆行列) [inverse_matrix_mod_2]
documentation_of: ../src/algorithm/math/linear_algebra/inverse_matrix_mod_2.hpp
---

$\mathbb{F}_2$上の正方行列をbitset Gaussian eliminationで反転する。
行交換と行のxorだけを使い、入力行列と単位行列を同時に掃き出す。

## `inverse_matrix_mod_2`

```cpp
auto inverse = inverse_matrix_mod_2<MAX_SIZE>(matrix, size);
```

`matrix`の先頭`size`行・先頭`size`列を正方行列として扱う。
逆行列が存在すれば`std::optional<std::vector<std::bitset<MAX_SIZE>>>`に入れて返し、
特異なら`std::nullopt`を返す。$0\times0$行列の逆行列は空の行列である。

`size > MAX_SIZE`または`matrix.size() != size`なら`std::invalid_argument`を送出する。
各行の`size`以降のbitは結果に影響しない。入力は値渡しであり、呼び出し元の行列は変更しない。

## API別の時間計算量・空間計算量

$N=\texttt{size}$、$L=\texttt{MAX_SIZE}$、$w$を機械語長とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `inverse_matrix_mod_2` | $O(N^2+N^2\lceil L/w\rceil)$ | $O(N\lceil L/w\rceil)$ |

$L=N$なら時間計算量は$O(N^3/w)$、追加領域は$O(N^2/w)$である。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
