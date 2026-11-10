---
title: Static Range Distinct Count Query (静的区間内の異なる値の個数クエリ) [DQUERY] [static_range_count_distinct]
documentation_of: ../../../../src/algorithm/range/static_query/static_range_count_distinct.hpp
---

静的配列の半開区間に含まれる相異なる値の個数を返す。
各位置について直前の同じ値の出現位置を求め、それを Wavelet Matrix に載せて数える。

## API

### テンプレート引数・コンストラクタ

```cpp
template<
    class T,
    int MAX_SIZE = -1,
    class Hash = std::hash<T>,
    class KeyEqual = std::equal_to<T>
>
class StaticRangeCountDistinct {
public:
    explicit StaticRangeCountDistinct(const vector<T>& values);
    int count(int left, int right) const;
    int count_distinct(int left, int right) const;
    int size() const noexcept;
};
```

```cpp
StaticRangeCountDistinct<int> dynamic(values);          // 実行時サイズに合わせた std::vector を使う
StaticRangeCountDistinct<int, 100000> fixed(values);    // 容量 100000 の固定長版
```

## テンプレート引数・引数・戻り値

- `T`: 配列の要素型。
- `MAX_SIZE`: 配列長の上限。省略時の `-1` は可変長版（実行時サイズに合わせて確保する）を表す。`0` 以上を指定すると、その容量の固定長版になる。
- `Hash`, `KeyEqual`: `T` に対するハッシュ関数と等値比較。
- `values`: 問い合わせ対象の静的な列。
- `left`, `right`: 0-indexed の半開区間 `[left, right)`。
- `count`, `count_distinct`: 区間に現れる相異なる値の個数を返す。
  `count_distinct` は `count` の別名である。空区間では `0` を返す。
- `size()`: 構築時の `values.size()` を返す。

## 時間計算量・空間計算量

$N$ を `values.size()`、$W=32$ を `int` のビット数とする。Wavelet Matrix の段数は可変長版でも $W$ である。
固定長版では $N$ の代わりに $\texttt{MAX_SIZE}$ 分の領域を確保・初期化する。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| コンストラクタ | 期待 $O(NW)$、ハッシュの最悪ケースで $O(NW+N^2)$ | $O(NW)$ |
| `count(left, right)`, `count_distinct(left, right)` | $O(W)$ | $O(1)$ |
| `size()` | $O(1)$ | $O(1)$ |

## 注意点

- `MAX_SIZE` を指定した場合、`values.size() > MAX_SIZE` なら `std::runtime_error` を送出する。
- `left < 0`、`right < left`、`right > size()` のいずれかなら `std::runtime_error` を送出する。
- `Hash` と `KeyEqual` は互いに整合し、同値な要素に同じハッシュ値を返す必要がある。
- `Hash` と `KeyEqual` はデフォルト構築可能でなければならない。
- 構築後の列は変更できない。
