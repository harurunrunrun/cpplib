---
title: Floating-Point Expansion Arithmetic Detail (浮動小数点展開算術の内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/numeric/expansion_arithmetic_detail.hpp
---

公開leafが共有する検査・誤差無し変換・展開演算の内部実装です。

## API

- `geometry3d_expansion_detail::validate(value)`: スカラーまたは展開の有限性を検査します。
- `canonical(low, high)`: 2成分を正規化した展開にします。
- `grow(expansion, value)`: 展開へ1成分を誤差を保持して加えます。
- `two_sum`, `two_diff`, `two_product`: 2スカラーを展開へ変換します。
- `sum`, `product`, `negate`, `scale`, `estimate`, `sign`: 公開APIの共通本体です。

## API別の時間計算量・空間計算量

成分数を $N,M$ とすると、`validate`、`negate`、`estimate`、`sign` は $O(N)$、`grow` は $O(N)$、`sum` は $O(M(N+M))$、`scale` は $O(N^2)$、`product` は $O(N^2M^2)$ です。2スカラー演算は $O(1)$ です。返却展開を除く追加空間は、各中間展開の大きさに比例します。

## 注意点

内部実装用headerです。利用側は対応する公開leafをincludeしてください。非有限値は `std::invalid_argument`、演算結果のoverflowは `std::overflow_error` になります。