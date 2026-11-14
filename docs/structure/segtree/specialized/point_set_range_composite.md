---
title: Point Set Range Composite (一点変更区間合成) [point_set_range_composite]
documentation_of: ../../../../src/structure/segtree/specialized/point_set_range_composite.hpp
---

アフィン関数の列に対して、1 点の変更と、半開区間内の関数を添字順に適用した値の取得を行う。

## PointSetRangeComposite

```cpp
template<class T, int MAX_SIZE = -1>
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

`MAX_SIZE` を省略すると可変長版の `Segtree` を内部で使い、長さの上限がなくなる。

## affine_type

$f(x) = \mathrm{multiplier} \cdot x + \mathrm{addend}$ を表す `MonoidAffine<T>` の別名。

## コンストラクタ

`size` 個の恒等関数からなる列、または `functions` を初期値とする列を構築する。固定容量の木はヒープ上に確保する。

## size

関数列の長さを返す。

## set

`index` 番目の関数を置き換える。3 引数版は `affine_type{multiplier, addend}` を設定する。

## evaluate

`left` 以上 `right` 未満の関数を、添字の昇順に `value` へ適用した結果を返す。空区間では `value` をそのまま返す。

## API 別の時間計算量・空間計算量

可変長版では $\texttt{MAX_SIZE}$ を列の長さに読み替える。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(\texttt{MAX_SIZE})$ | $O(\texttt{MAX_SIZE})$ |
| `size()` | $O(1)$ | $O(1)$ |
| `set(...)` | $O(\log \texttt{MAX_SIZE})$ | $O(1)$ |
| `evaluate(...)` | $O(\log \texttt{MAX_SIZE})$ | $O(1)$ |

## 注意点

- 固定容量版では $0 \le \mathrm{size} \le \texttt{MAX_SIZE}$ が必要である。
- 点は $[0, \mathrm{size})$、区間は `0 <= left <= right <= size()` でなければならない。違反時は `std::runtime_error` を送出する。
- 合成の順序は左から右であり、可換性を仮定しない。
- `T` は `0`, `1` から構築でき、加算と乗算に対応する必要がある。すべての演算結果は `T` で表現できなければならない。
- オブジェクトはコピーできない。
