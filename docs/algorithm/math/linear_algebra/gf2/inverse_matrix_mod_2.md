---
title: Inverse Matrix (Mod 2) (二元体上の逆行列) [inverse_matrix_mod_2]
documentation_of: ../../../../../src/algorithm/math/linear_algebra/gf2/inverse_matrix_mod_2.hpp
---

$\mathbb{F}_2$ 上の正方行列を bitset による Gauss の消去法で反転する。行交換と行の xor だけを使い、入力行列と単位行列を同時に掃き出す。

## `inverse_matrix_mod_2`

```cpp
auto inverse = inverse_matrix_mod_2<MAX_SIZE>(matrix, size);
```

`matrix`（`std::vector<std::bitset<MAX_SIZE>>`）の先頭 `size` 行・先頭 `size` 列を正方行列として扱う。逆行列が存在すれば `std::optional<std::vector<std::bitset<MAX_SIZE>>>` に入れて返し、特異なら `std::nullopt` を返す。$0\times0$ 行列の逆行列は空の行列である。

`size > MAX_SIZE` または `matrix.size() != size` なら `std::invalid_argument` を送出する。各行の `size` 以降の bit は結果に影響しない。入力は値渡しであり、呼び出し元の行列は変更しない。

```cpp
std::vector<std::vector<bool>> values{ {true, true}, {true, false} };
auto inverse = inverse_matrix_mod_2(values);        // 可変長版
auto inverse2 = inverse_matrix_mod_2(values, 2);    // size を明示
```

容量のテンプレート引数を省略した可変長版は、入出力に二次元の `std::vector<bool>` を使い、内部を 64 bit 単位に詰めて処理する。`values.size() != size` なら `std::invalid_argument` を送出する。

## 時間計算量・空間計算量

$N$ を `size`、$L$ を `MAX_SIZE`、$w$ を機械語の bit 数とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `inverse_matrix_mod_2`（bitset 版） | $O(N^2+N^2\lceil L/w\rceil)$ | $O(N\lceil L/w\rceil)$ |
| `inverse_matrix_mod_2`（可変長版） | $O(N^2\lceil N/64\rceil)$ | $O(N\lceil N/64\rceil+N^2)$ |

$L=N$ なら bitset 版の時間計算量は $O(N^3/w)$、追加領域は $O(N^2/w)$ である。

## 注意点

- 可変長版で各行の長さが `size` に満たない場合は `std::invalid_argument` を送出する。
