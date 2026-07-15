---
title: Floating-Point Expansion Arithmetic (浮動小数点展開演算)
documentation_of: ../src/algorithm/geometry/3d/expansion_arithmetic.hpp
---

## 公開型

- `Geometry3DExpansion`: 小さい成分から並ぶ `vector<long double>`。

## API

- `geometry3d_two_sum(a,b)` / `geometry3d_two_diff(a,b)`: 和・差を最大2成分へ展開する。
- `geometry3d_two_product(a,b)`: `std::fma` を使い積と丸め残差を返す。
- `geometry3d_expansion_sum(a,b)`: 展開同士の和。
- `geometry3d_expansion_negate(a)`: 符号反転。
- `geometry3d_expansion_scale(a,s)`: scalar倍。
- `geometry3d_expansion_product(a,b)`: 展開同士の積。
- `geometry3d_expansion_estimate(a)`: 成分和による近似値。
- `geometry3d_expansion_sign(a)`: 最上位非零成分の符号。

## API別の時間計算量・空間計算量

成分数を $N,M$ とする。two-sum/diff/productは $O(1)$。negate/estimate/signは $O(N)$。sumは $O(NM)$、scaleは最悪 $O(N^2)$、productは最悪 $O(N^2M^2)$。戻り値を含む領域は生成成分数に比例する。

## 注意点

入力展開は絶対値が小さい非零成分から順に並べ、成分を重複させない。入力成分は有限でなければならず、非有限値は `std::invalid_argument`、演算overflowは `std::overflow_error`。underflowで消えた情報と正確変換前に丸められた入力情報は復元できない。適応的述語は不確実な場合に正確なdyadic多倍長計算で符号を確定する。
