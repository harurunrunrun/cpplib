---
title: Range Affine Point Get (区間affine作用一点取得) [range_affine_point_get]
documentation_of: ../../../../src/structure/segtree/specialized/range_affine_point_get.hpp
---

値の列に対して、区間アフィン作用と 1 点取得を行う。

## RangeAffinePointGet

```cpp
template<class T, int MAX_SIZE = -1>
class RangeAffinePointGet {
public:
    using affine_type = MonoidAffine<T>;
    explicit RangeAffinePointGet(const vector<T>& values);
    int size() const;
    void apply(int left, int right, affine_type function);
    void apply(int left, int right, T multiplier, T addend);
    T get(int index);
};
```

内部では `DualSegtree` を使う。`MAX_SIZE` を省略すると可変長版になり、長さの上限がなくなる。

## affine_type

$f(x) = \mathrm{multiplier} \cdot x + \mathrm{addend}$ を表す `MonoidAffine<T>` の別名。

## コンストラクタ

`values` を初期値として構築する。固定容量の木はヒープ上に確保する。

## size

値の列の長さを返す。

## apply

半開区間 $[left, right)$ の各値 $x$ を `function.multiplier * x + function.addend` に変更する。4 引数版も同じ作用を表す。

## get

`index` 番目の値に保留中の作用を反映し、その値を返す。

## API 別の時間計算量・空間計算量

可変長版では $\texttt{MAX_SIZE}$ を列の長さに読み替える。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(\texttt{MAX_SIZE})$ | $O(\texttt{MAX_SIZE})$ |
| `size()` | $O(1)$ | $O(1)$ |
| `apply(...)` | $O(\log \texttt{MAX_SIZE})$ | $O(1)$ |
| `get(...)` | $O(\log \texttt{MAX_SIZE})$ | $O(1)$ |

## 注意点

- 固定容量版では `values.size() <= MAX_SIZE` が必要である。
- 点は $[0, \mathrm{size})$、区間は `0 <= left <= right <= size()` でなければならない。違反時は `std::runtime_error` を送出する。
- 後から指定した作用は、それ以前の作用の後に適用される。
- `T` は `0`, `1` と区間長から構築でき、加算と乗算に対応する必要がある。
- オブジェクトはコピーできない。
