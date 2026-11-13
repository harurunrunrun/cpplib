---
title: Range Set Range Composite (区間代入区間合成) [range_set_range_composite]
documentation_of: ../../../../src/structure/segtree/specialized/range_set_range_composite.hpp
---

アフィン関数の列に対して、区間の一様代入と、区間内の関数を添字順に適用した値の取得を行う。

## RangeSetRangeComposite

```cpp
template<class T, int MAX_SIZE = -1>
class RangeSetRangeComposite {
public:
    using affine_type = MonoidAffine<T>;
    explicit RangeSetRangeComposite(const vector<affine_type>& functions);
    int size() const;
    void assign(int left, int right, affine_type function);
    void assign(int left, int right, T multiplier, T addend);
    T evaluate(int left, int right, T value);
};
```

内部では `LazySegtree` を使う。`MAX_SIZE` を省略すると可変長版になり、長さの上限がなくなる。

## affine_type

$f(x) = \mathrm{multiplier} \cdot x + \mathrm{addend}$ を表す `MonoidAffine<T>` の別名。

## コンストラクタ

`functions` を初期値として構築する。固定容量の木はヒープ上に確保する。

## size

関数列の長さを返す。

## assign

半開区間 $[left, right)$ のすべての関数を同じ `function` に置き換える。4 引数版は `affine_type{multiplier, addend}` を設定する。

## evaluate

半開区間 $[left, right)$ の関数を、添字の昇順に `value` へ適用した結果を返す。空区間では `value` を返す。遅延作用を押し下げるため `const` ではない。

## API 別の時間計算量・空間計算量

可変長版では $\texttt{MAX_SIZE}$ を列の長さに読み替える。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(\texttt{MAX_SIZE})$ | $O(\texttt{MAX_SIZE})$ |
| `size()` | $O(1)$ | $O(1)$ |
| `assign(...)` | $O(\log^2 \texttt{MAX_SIZE})$ | $O(1)$ |
| `evaluate(...)` | $O(\log^2 \texttt{MAX_SIZE})$ | $O(1)$ |

同じ関数を区間長の回数だけ合成した結果を二分累乗で求めるため、遅延作用 1 回の反映に $O(\log \texttt{MAX_SIZE})$ 時間を使う。

## 注意点

- 固定容量版では `functions.size() <= MAX_SIZE` が必要である。
- 区間は `0 <= left <= right <= size()` でなければならない。違反時は `std::runtime_error` を送出する。
- 合成の順序は左から右であり、可換性を仮定しない。
- `T` は `0`, `1` から構築でき、加算と乗算に対応する必要がある。すべての演算結果は `T` で表現できなければならない。
- オブジェクトはコピーできない。
