---
title: Minimum Insertions to Palindrome (回文化の最小挿入回数) [AIBOHP] [IOIPALIN]
documentation_of: ../src/algorithm/string/minimum_insertions_to_palindrome.hpp
---

列に要素を挿入して回文にするために必要な最小挿入回数を求める。
挿入位置は先頭・末尾・途中のいずれでもよく、元の列の要素の順序は変えない。

## API

```cpp
template<class Sequence>
int minimum_insertions_to_palindrome(const Sequence& sequence)
```

## 引数・戻り値

- `sequence`: 元の順序を保ったまま回文化する列。
- 戻り値: 必要な挿入回数の最小値。空列またはすでに回文である列では `0`。

## API別の時間計算量・空間計算量

$N=sequence.size()$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_insertions_to_palindrome(sequence)` | $O(N^2)$ | $O(N)$ |

## 注意点

- 空列では `0` を返し、すでに回文なら `0` を返す。
- 挿入する要素列や完成した回文そのものは復元しない。
- 要素の比較には `operator==` を使うため、文字列では大文字と小文字を区別する。
- `Sequence` はコピー、`size()`、ランダムアクセス、可変イテレータによる反転をサポートする必要がある。
- 列長は `int` に収まる必要がある。
