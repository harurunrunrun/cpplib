---
title: Range Affine Range Sum (区間affine作用区間和) [range_affine_range_sum]
documentation_of: ../../../../src/structure/segtree/specialized/range_affine_range_sum.hpp
---

値列への区間affine作用と区間和取得を行う。

## RangeAffineRangeSum

```cpp
template<class T, int MAX_SIZE>
class RangeAffineRangeSum {
public:
    using affine_type = MonoidAffine<T>;

    explicit RangeAffineRangeSum(const vector<T>& values);

    int size() const;
    void apply(int left, int right, affine_type function);
    void apply(int left, int right, T multiplier, T addend);
    T range_sum(int left, int right);
};
```

## affine_type

`multiplier*x+addend` を表す `MonoidAffine<T>` のalias。

## Constructor

`values` を初期値として構築する。固定容量treeはheap上に確保する。

## size

値列の長さを返す。

## apply

`[left,right)` の各値 `x` を
`function.multiplier*x+function.addend` に変更する。4引数版も同じ作用を表す。

## range_sum

`[left,right)` の値の和を返す。空区間では `T(0)` を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(\mathtt{MAX\_SIZE})$ | $O(\mathtt{MAX\_SIZE})$ |
| `size()` | $O(1)$ | $O(1)$ |
| `apply(...)` | $O(\log \mathtt{MAX\_SIZE})$ | $O(1)$ |
| `range_sum(...)` | $O(\log \mathtt{MAX\_SIZE})$ | $O(1)$ |

## 注意点

- `values.size() <= MAX_SIZE` が必要である。
- 区間は `0 <= left <= right <= size()` でなければならない。
  違反時は `runtime_error` を送出する。
- 後から指定した作用は、それ以前の作用の後に適用される。
- `T` は `0`、`1` と区間長から構築でき、加算と乗算に対応する必要がある。
  区間和を含むすべての結果は `T` で表現できなければならない。
- objectのcopyはできない。

## 容量を省略する場合

容量引数の既定値は `-1`。`RangeAffineRangeSum<T> tree(values);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
