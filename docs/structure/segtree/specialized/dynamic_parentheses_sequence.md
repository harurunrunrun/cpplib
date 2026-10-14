---
title: Dynamic Parentheses Sequence (動的括弧列) [BRCKTS]
documentation_of: ../../../../src/structure/segtree/specialized/dynamic_parentheses_sequence.hpp
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

## 容量を省略する場合

容量引数の既定値は `-1`。`DynamicParenthesesSequence<> tree(values);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
