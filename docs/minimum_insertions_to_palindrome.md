---
title: Minimum Insertions to Palindrome (回文化の最小挿入回数) [AIBOHP] [IOIPALIN]
documentation_of: ../src/algorithm/string/palindrome/minimum_insertions_to_palindrome.hpp
---

列に要素を挿入して回文にするために必要な最小挿入回数を求める。
挿入位置は先頭・末尾・途中のいずれでもよく、元の列の要素の順序は変えない。

## API

```cpp
std::size_t minimum_insertions_to_palindrome_bytes(std::string_view sequence)
int minimum_insertions_to_palindrome(std::string_view sequence)
int minimum_insertions_to_palindrome(const std::string& sequence)
template<class Sequence>
int minimum_insertions_to_palindrome(const Sequence& sequence)
```

## 引数・戻り値

- `minimum_insertions_to_palindrome_bytes`: byte 列について bit-parallel LCS
  を使い、必要な挿入回数を `std::size_t` で返す。埋め込み NUL を含む
  全 256 byte 値を扱える。
- `minimum_insertions_to_palindrome(std::string_view)`:
  byte 版を利用し、既存APIと同じ `int` で返す。
- `minimum_insertions_to_palindrome(const std::string&)`:
  `std::string_view` overload へ委譲する。
- 汎用 template: 要素を `operator==` で比較できる列に通常の LCS DP を使う。

どの API も元の順序を保ったまま回文化するときの最小挿入回数を返す。
空列またはすでに回文である列では `0` を返す。

## API別の時間計算量・空間計算量

$N=sequence.size()$ とし、機械語長を 64 bit とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_insertions_to_palindrome_bytes(sequence)` | $O(N\lceil N/64\rceil+N)$ | $O(256\lceil N/64\rceil+N)$ |
| `minimum_insertions_to_palindrome(std::string_view)` | $O(N\lceil N/64\rceil+N)$ | $O(256\lceil N/64\rceil+N)$ |
| `minimum_insertions_to_palindrome(const std::string&)` | $O(N\lceil N/64\rceil+N)$ | $O(256\lceil N/64\rceil+N)$ |
| 汎用 `minimum_insertions_to_palindrome(sequence)` | $O(N^2)$ | $O(N)$ |

## 注意点

- 挿入する要素列や完成した回文そのものは復元しない。
- byte 版は入力を変更せず、反転したコピーを内部で 1 つ作る。
- `int` を返す 2 overload は列長が `int` に収まらない場合
  `std::length_error` を投げる。
- 汎用 template の `Sequence` はコピー、`size()`、ランダムアクセス、
  可変イテレータによる反転をサポートし、列長は `int` に収まる必要がある。
