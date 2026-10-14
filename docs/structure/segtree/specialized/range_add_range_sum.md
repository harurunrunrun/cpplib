---
title: Range Add Range Sum (区間加算・区間和) [CDC12_H] [HORRIBLE]
documentation_of: ../../../../src/structure/segtree/specialized/range_add_range_sum.hpp
---

零初期化された列への区間加算と区間和を管理する。

## RangeAddRangeSum

```cpp
template<class T, int MAX_SIZE>
class RangeAddRangeSum {
public:
    explicit RangeAddRangeSum(int size);
    int size() const;
    void add(int left, int right, T value);
    T sum(int left, int right) const;
};
```

## Constructor

長さ `size` の零列を構築する。

## size

列長を返す。

## add

半開区間 `[left,right)` の各要素に `value` を加える。

## sum

半開区間 `[left,right)` の総和を返す。

## API別の時間計算量・空間計算量

列長を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `add(l,r,x)` | $O(\log N)$ | $O(1)$ |
| `sum(l,r)` | $O(\log N)$ | $O(1)$ |

## 注意点

- `size` は `MAX_SIZE` 以下、区間は `0 <= left <= right <= size()` でなければならない。
- 加算および総和は `T` で表現できる必要がある。
- 前提違反時は `runtime_error` を送出する。

## 容量を省略する場合

容量引数の既定値は `-1`。`RangeAddRangeSum<T> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
