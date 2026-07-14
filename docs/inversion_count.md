---
title: Inversion Count (転倒数) [INVCNT]
documentation_of: ../src/algorithm/other/inversion_count.hpp
---

比較関数で定まる列の転倒数を返す。

## API

```cpp
template<class T, class Compare = std::less<T>>
std::uint64_t inversion_count(
    const std::vector<T>& values,
    Compare compare = Compare{}
)
```

## 引数・戻り値

- `values`: 対象の列。関数は列を変更しない。
- `compare`: 要素の順序を定める比較関数。省略時は `std::less<T>`。
- 戻り値: $i<j$ かつ `compare(values[j], values[i])` である組 $(i,j)$ の個数。
  比較上等価な2要素は含めない。空列と長さ1の列では `0` を返す。

## API別の時間計算量・空間計算量

$N=values.size()$ とし、比較1回を $O(1)$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `inversion_count(values)` | $O(N \log N)$ | $O(N)$ |
| `inversion_count(values,compare)` | $O(N \log N)$ | $O(N)$ |

## 注意点

- `compare`はstrict weak orderingを満たす必要がある。
- 転倒数は `std::uint64_t` に収まる必要がある。
- 実行中に`values`と`compare`が参照する順序を変更してはならない。
- `compare`が送出した例外はそのまま呼び出し元へ伝わる。
