---
title: Safe Count Arithmetic (安全な個数演算)
documentation_of: ../src/approximate/evolutionary/detail/safe_count.hpp
---

# Safe Count Arithmetic (安全な個数演算)

`std::size_t` の加算・乗算をoverflow検査付きで行う内部API。

## API

### `checked_add(lhs, rhs, message)`

`lhs+rhs` を返す。時間計算量・追加空間計算量は $O(1)$。

### `checked_multiply(lhs, rhs, message)`

`lhs*rhs` を返す。時間計算量・追加空間計算量は $O(1)$。

### `checked_multiply_add(lhs, rhs, addend, message)`

`lhs*rhs+addend` を返す。時間計算量・追加空間計算量は $O(1)$。

## 注意点

`approximate::evolutionary::detail` の内部APIである。結果が `std::size_t` に収まらない場合は、指定したmessageで `std::length_error` を送出する。
