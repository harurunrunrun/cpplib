---
title: Floating-Point Expansion Arithmetic Detail (浮動小数点展開算術の内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/numeric/expansion_arithmetic_detail.hpp
---

公開されている個別ヘッダが共有する、検査・誤差のない変換・展開（expansion）演算の内部実装である。

## API

名前空間 `geometry3d_expansion_detail` に次の関数を置く。

- `validate(value)`: スカラーまたは展開の各成分が有限であることを検査する。
- `canonical(low, high)`: 2 成分を、零成分を除いた展開にする（全成分が零なら `{0}`）。
- `grow(expansion, value)`: 展開に 1 成分を誤差なしで加える。
- `two_sum`, `two_diff`, `two_product`: 2 つのスカラーの和・差・積を誤差なしで展開に変換する。
- `sum`, `product`, `negate`, `scale`, `estimate`, `sign`: 対応する公開 API の共通本体（展開同士の和・積、符号反転、スカラー倍、近似値、符号）。

## API別の時間計算量・空間計算量

成分数を $N, M$ とする。

- `validate`, `negate`, `estimate`, `sign`, `grow`: $O(N)$。
- `sum`: `grow` を $M$ 回繰り返すため $O(M(N+M))$。
- `scale`: $O(N^2)$。
- `product`: $O(N^2M^2)$。
- 2 スカラーの演算: $O(1)$。

返り値の展開を除く追加領域は、各中間展開の大きさに比例する。

## 注意点

内部実装用のヘッダである。利用側は対応する公開ヘッダを include すること。非有限な成分には `std::invalid_argument`、演算結果のオーバーフローには `std::overflow_error` を送出する。
