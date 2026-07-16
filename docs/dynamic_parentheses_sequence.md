---
title: Dynamic Parentheses Sequence (動的括弧列) [BRCKTS]
documentation_of: ../src/structure/segtree/dynamic_parentheses_sequence.hpp
---

丸括弧列の1点反転と、列全体が正しい括弧列かの判定を行う。

## DynamicParenthesesSequence

```cpp
template<int MAX_SIZE>
class DynamicParenthesesSequence {
public:
    explicit DynamicParenthesesSequence(string sequence);
    int size() const;
    void toggle(int position);
    bool balanced() const;
};
```

## Constructor

`'('` と `')'` だけからなる列を構築する。

## size

列長を返す。

## toggle

指定位置の括弧を反転する。

## balanced

列全体が正しい括弧列なら真を返す。

## API別の時間計算量・空間計算量

列長を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `toggle(position)` | $O(\log N)$ | $O(1)$ |
| `balanced()` | $O(1)$ | $O(1)$ |

## 注意点

- 入力文字列は丸括弧のみで、長さは `MAX_SIZE` 以下でなければならない。
- 位置は $[0,N)$ で指定する。
- 前提違反時は `runtime_error` を送出する。
