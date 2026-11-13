---
title: Safe Count Arithmetic (安全な個数演算)
documentation_of: ../../../../src/approximate/evolutionary/detail/safe_count.hpp
---

`std::size_t` の加算・乗算をオーバーフロー検査付きで行う内部 API である。

## API

### `checked_add(lhs, rhs, message)`

`lhs + rhs` を返す。時間計算量・追加空間計算量はともに $O(1)$ である。

### `checked_multiply(lhs, rhs, message)`

`lhs * rhs` を返す。時間計算量・追加空間計算量はともに $O(1)$ である。

### `checked_multiply_add(lhs, rhs, addend, message)`

`lhs * rhs + addend` を返す。時間計算量・追加空間計算量はともに $O(1)$ である。

## 注意点

- `approximate::evolutionary::detail` の内部 API である。
- 結果が `std::size_t` に収まらない場合は、指定した `message` を持つ `std::length_error` を送出する。
