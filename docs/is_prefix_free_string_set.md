---
title: Prefix-Free String Set Test (接頭辞自由文字列集合判定) [PHONELST]
documentation_of: ../src/algorithm/string/automata/is_prefix_free_string_set.hpp
---

文字列列の異なる2要素について、一方が他方の接頭辞にならないかを判定する。

## is_prefix_free_string_set

```cpp
bool is_prefix_free_string_set(
    const vector<string>& strings
);
```

## 引数

- `strings`: 判定する文字列列。

## 戻り値

異なる2要素の一方が他方の接頭辞になる組がなく、重複もなければ `true`。それ以外は `false`。

## API別の時間計算量・空間計算量

要素数を $N$、文字列の最大長を $L$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `is_prefix_free_string_set` | $O(NL\log(N+1))$ | $O(N)$ |

## 注意点

- 入力列と各文字列は変更しない。
- 空文字列は任意の文字列の接頭辞である。空文字列だけからなる1要素の列は接頭辞自由である。
- 同じ文字列が複数回現れる場合は `false` を返す。
