---
title: Diff (差分)
documentation_of: ../../../../src/algorithm/string/sequence/diff.hpp
---

2 つの列の差分（最短編集スクリプト）を求める。Myers 法を用い、記録が上限を超えたら Hirschberg 法に切り替える。

## 関数

```cpp
template<class Sequence>
vector<DiffOp<typename Sequence::value_type>> diff_sequence(const Sequence& a, const Sequence& b)
```

`DiffOp<T>` は次のフィールドを持つ。

```cpp
DiffTag tag;
T value;
```

`tag` は `DiffTag::Equal`、`DiffTag::Insert`、`DiffTag::Erase` のいずれかである。
`a` に先頭から操作列を適用すると `b` になる（`Equal` と `Erase` は `a` の要素、`Insert` は `b` の要素を `value` に持つ）。
挿入と削除の総数は最小になる。

## 時間計算量・空間計算量

$N=\lvert a\rvert$、$M=\lvert b\rvert$、最短編集距離（挿入と削除の数）を $D$、返す操作数を $L$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `diff_sequence(a, b)`（Myers 法の記録を使う場合） | $O((N+M)D+L)$ | $O(N+M+D^2+L)$ |
| `diff_sequence(a, b)`（記録の上限を超えた場合） | $O(NM+L)$ | $O(N+M+L)$ |
| `DiffOp::tag` / `DiffOp::value` の参照 | $O(1)$ | - |

Myers 法の記録は実装上 $8388608$ 要素に制限され、超えると Hirschberg 法に切り替わる。どちらも最大 $N+M$ 個の操作を列挙する。

## 注意点

- 要素は `operator==` で比較する。
- 最短の編集スクリプトが複数ある場合、そのうちの 1 つを返す。
