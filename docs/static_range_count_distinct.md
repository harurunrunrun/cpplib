---
title: Static Range Distinct Count Query (静的区間内の異なる値の個数クエリ) [DQUERY] [static_range_count_distinct]
documentation_of: ../src/algorithm/range/static_range_count_distinct.hpp
---

静的配列の半開区間に含まれる相異なる値の個数を返す。

## API

### テンプレート引数・コンストラクタ

```cpp
template<
    class T,
    int MAX_SIZE,
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

### `count(left,right)` / `count_distinct(left,right)`

```cpp
int count(int left, int right) const
int count_distinct(int left, int right) const
```

### `size()`

```cpp
int size() const noexcept
```

## テンプレート引数・引数・戻り値

- `T`: 配列要素型。
- `MAX_SIZE`: 配列長の上限。0以上でなければならない。
- `Hash`, `KeyEqual`: `T` に対するhash関数と等値比較。
- `values`: 問い合わせ対象となる静的列。
- `left`, `right`: 0-indexed半開区間 `[left,right)`。
- `count`, `count_distinct`: 区間に現れる相異なる値数。
  `count_distinct` は `count` の別名で、空区間では `0`。
- `size()`: 構築時の `values.size()`。

## API別の時間計算量・空間計算量

$N=values.size()$、$W$ を `int` のbit数とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| コンストラクタ | expected $O(MAX_SIZE\cdot W+N)$、hash worst case $O(MAX_SIZE\cdot W+N^2)$ | $O(MAX_SIZE\cdot W+N)$ |
| `count(left,right)`, `count_distinct(left,right)` | $O(W)$ | $O(1)$ |
| `size()` | $O(1)$ | $O(1)$ |

## 注意点

- `values.size() > MAX_SIZE` なら `runtime_error` を送出する。
- `left < 0`、`right < left`、`right > size()` のいずれかなら
  `runtime_error` を送出する。
- `Hash` と `KeyEqual` は互いに整合し、同値な要素へ同じhash値を返す必要がある。
- `Hash` と `KeyEqual` はデフォルト構築可能でなければならない。
- 構築後の列は変更できない。
