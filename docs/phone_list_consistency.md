---
title: Phone List Consistency (電話番号リスト整合性) [PHONELST]
documentation_of: ../src/algorithm/string/phone_list_consistency.hpp
---

電話番号の列について、ある要素が別の要素の接頭辞になっていないかを判定する。

# `phone_list_is_consistent`

```cpp
bool phone_list_is_consistent(const vector<string>& phone_numbers);
```

## 引数

- `phone_numbers`: 判定する電話番号の列。

## 戻り値

異なる2要素の一方が他方の接頭辞でなく、重複もなければ `true`。それ以外は `false`。

## API別の時間計算量・空間計算量

要素数を $N$、電話番号の最大長を $L$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `phone_list_is_consistent` | $O(NL\log N)$ | $O(N)$ |

## 注意点

- 入力列と各文字列は変更しない。
- 空文字列はすべての文字列の接頭辞として扱う。空文字列だけを1個含む列は整合する。
- 同じ電話番号が複数回現れる場合は整合しない。
