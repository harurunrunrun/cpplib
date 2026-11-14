---
title: Dynamic Parentheses Sequence (動的括弧列) [BRCKTS]
documentation_of: ../../../../src/structure/segtree/specialized/dynamic_parentheses_sequence.hpp
---

丸括弧列の 1 点反転と、列全体が正しい括弧列かどうかの判定を行う。

## DynamicParenthesesSequence

```cpp
template<int MAX_SIZE = -1>
class DynamicParenthesesSequence {
public:
    explicit DynamicParenthesesSequence(string sequence);
    int size() const;
    void toggle(int position);
    bool balanced() const;
};
```

`MAX_SIZE` を省略すると可変長版の `Segtree` を内部で使い、`DynamicParenthesesSequence<> seq(s);` のように長さの上限なしで構築できる。

## コンストラクタ

`'('` と `')'` だけからなる列から構築する。

## size

列の長さを返す。

## toggle

指定した位置の括弧を反転する（`'('` と `')'` を入れ替える）。

## balanced

列全体が正しい括弧列なら `true` を返す。空列も正しい括弧列とみなす。

## API 別の時間計算量・空間計算量

列の長さを $N$ とする。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `toggle(position)` | $O(\log N)$ | $O(1)$ |
| `balanced()` | $O(1)$ | $O(1)$ |

## 注意点

- 入力文字列は丸括弧のみからなり、固定容量版では長さが `MAX_SIZE` 以下でなければならない。
- 位置は $[0, N)$ で指定する。
- 括弧以外の文字や範囲外の位置では `std::runtime_error` を送出する。
