---
title: Diff (差分)
documentation_of: ../src/algorithm/string/diff.hpp
---

2 つの列の差分を LCS に基づいて求める。

## 関数

```cpp
vector<DiffOp<T>> diff_sequence(a, b)
```

`DiffOp<T>` は次を持つ。

```cpp
DiffTag tag;
T value;
```

`tag` は `Equal`, `Insert`, `Erase` のいずれか。

## 時間計算量

編集距離を $D$ とする。

- 通常: $O((N+M)D)$
- trace容量の上限を超えた場合: Hirschberg法に切り替え、$O(NM)$ 時間・既存配列を含め $O(N+M)$ 補助領域

## API別の時間計算量・空間計算量

$N=|a|$, $M=|b|$、最短編集距離を $D$、返す操作数を $L$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `diff_sequence(a,b)`（Myers trace使用時） | $O((N+M)D+L)$ | $O(N+M+D^2+L)$ |
| `diff_sequence(a,b)`（trace上限超過後） | $O(NM+L)$ | $O(N+M+L)$ |
| `DiffOp::tag` / `DiffOp::value` の参照 | $O(1)$ | - |

traceは実装上 8,388,608 entries に制限され、超えるとHirschberg法へ切り替わる。どちらも最大 $N+M$ 個の操作を列挙する。

## 注意点

文字範囲、alphabet、固定容量、構築後の更新可否は各APIの説明に従う。返すindexと区間は0-indexedである。
