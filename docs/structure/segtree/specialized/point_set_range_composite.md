---
title: Point Set Range Composite (一点変更区間合成) [point_set_range_composite]
documentation_of: ../../../../src/structure/segtree/specialized/point_set_range_composite.hpp
---

affine関数列の一点変更と、半開区間内の関数を添字順に適用した値の取得を行う。

## PointSetRangeComposite

```cpp
template<class T, int MAX_SIZE>
class PointSetRangeComposite {
public:
    using affine_type = MonoidAffine<T>;

    explicit PointSetRangeComposite(int size);
    explicit PointSetRangeComposite(const vector<affine_type>& functions);

    int size() const;
    void set(int index, affine_type function);
    void set(int index, T multiplier, T addend);
    T evaluate(int left, int right, T value) const;
};
```

## affine_type

`multiplier*x+addend` を表す `MonoidAffine<T>` のalias。

## Constructor

`size` 個の恒等関数を持つ列、または `functions` を初期値とする列を構築する。
固定容量treeはheap上に確保する。

## size

関数列の長さを返す。

## set

`index` 番目の関数を置換する。3引数版は
`affine_type{multiplier, addend}` を設定する。

## evaluate

`left` 以上 `right` 未満の関数を添字の昇順で `value` へ適用した結果を返す。
空区間では `value` をそのまま返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(\mathtt{MAX\_SIZE})$ | $O(\mathtt{MAX\_SIZE})$ |
| `size()` | $O(1)$ | $O(1)$ |
| `set(...)` | $O(\log \mathtt{MAX\_SIZE})$ | $O(1)$ |
| `evaluate(...)` | $O(\log \mathtt{MAX\_SIZE})$ | $O(1)$ |

## 注意点

- $0\leq size\leq\mathtt{MAX\_SIZE}$ が必要である。
- 点は `[0,size())`、区間は `0 <= left <= right <= size()` でなければならない。
  違反時は `runtime_error` を送出する。
- 合成順序は左から右であり、可換性を仮定しない。
- `T` は `0`、`1` から構築でき、加算と乗算に対応する必要がある。
  すべての演算結果は `T` で表現できなければならない。
- objectのcopyはできない。

## 容量を省略する場合

容量引数の既定値は `-1`。`PointSetRangeComposite<T> tree(values);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
