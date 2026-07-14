---
title: Inversion Count (転倒数)
documentation_of: ../src/algorithm/other/inversion_count.hpp
---

列の転倒数を安定merge sortで数える。

## `inversion_count(values, compare)`

```cpp
std::uint64_t count = inversion_count(values);
std::uint64_t count = inversion_count(values, compare);
```

`i < j`かつ`compare(values[j], values[i])`である組 `(i, j)` の個数を返す。
`compare`を省略すると`std::less<T>`を使う。比較上等価な2要素は転倒に含めない。
要素自体は複製せず、index列をmergeする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `inversion_count(values)` | $O(N \log N)$ | $O(N)$ |
| `inversion_count(values, compare)` | $O(N \log N)$ | $O(N)$ |

空列と長さ1の列では0を返す。

## 注意点

- `compare`はstrict weak orderingを満たす必要がある。
- 戻り値の型は`std::uint64_t`である。
- 実行中に`values`と`compare`が参照する順序を変更してはならない。
- `compare`が送出した例外はそのまま呼び出し元へ伝わる。
