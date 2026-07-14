---
title: Integer Set (整数集合) [CTRICK] [ORDERS] [predecessor_problem]
documentation_of: ../src/structure/tree/integer_set.hpp
---

`[0, MAX_SIZE)` の整数を管理する集合です。各ノードを16分岐にした多分木で、要素数の累積和をノード内に保持します。

```cpp
template<class L, L MAX_SIZE>
struct Int_Set;
```

- `L` は `unsigned int` または `unsigned long long`
- `0 < MAX_SIZE <= std::numeric_limits<int>::max()`

x86環境では実行時にAVX-512、AVX2の順に利用可能か判定し、どちらも利用できない場合はscalar実装を使います。AVXを有効にするコンパイルオプションは不要です。

以下で $h = O(\log_{16} MAX_SIZE)$ とします。`kth_*` の `k` は0始まりです。戻り値が存在しない操作は `std::nullopt` を返します。

default constructorとmove constructorは $O(1)$ です。

# insert

```cpp
void insert(L x)
```

`x` を挿入します。すでに存在する場合は何もしません。`x` が範囲外の場合は `std::runtime_error` を送出します。

計算量: $O(h)$

# erase

```cpp
void erase(L x)
```

`x` を削除します。存在しない場合は何もしません。`x` が範囲外の場合は `std::runtime_error` を送出します。

計算量: $O(h)$

# contain

```cpp
bool contain(L x)
```

`x` が存在するかを返します。範囲外では `false` を返します。

計算量: $O(h)$

# range_sum

```cpp
L range_sum(L l, L r)
```

`[l, r)` に含まれる要素数を返します。区間の範囲外部分は `[0, MAX_SIZE)` に切り詰められます。

計算量: $O(h)$

# least / more

```cpp
std::optional<L> least(L x)
std::optional<L> more(L x)
```

- `least(x)`: `x <= y` を満たす最小の要素 `y`
- `more(x)`: `x < y` を満たす最小の要素 `y`

計算量: $O(h)$

# most / less

```cpp
std::optional<L> most(L x)
std::optional<L> less(L x)
```

- `most(x)`: `y <= x` を満たす最大の要素 `y`
- `less(x)`: `y < x` を満たす最大の要素 `y`

計算量: $O(h)$

# kth_ge / kth_gt

```cpp
std::optional<L> kth_ge(L x, L k)
std::optional<L> kth_gt(L x, L k)
```

- `kth_ge(x, k)`: `x <= y` を満たす要素を昇順に並べたときの `k` 番目
- `kth_gt(x, k)`: `x < y` を満たす要素を昇順に並べたときの `k` 番目

計算量: $O(16h)$

# kth_le / kth_lt

```cpp
std::optional<L> kth_le(L x, L k)
std::optional<L> kth_lt(L x, L k)
```

- `kth_le(x, k)`: `y <= x` を満たす要素を降順に並べたときの `k` 番目
- `kth_lt(x, k)`: `y < x` を満たす要素を降順に並べたときの `k` 番目

計算量: $O(16h)$

`kth_*` には符号付きの `x` と `k` を受け取るオーバーロードもあります。負の `k` に対しては `std::nullopt` を返します。

# min / max / empty

```cpp
std::optional<L> min()
std::optional<L> max()
bool empty()
```

最小要素、最大要素、集合が空かを返します。`min` と `max` は $O(h)$、`empty` は $O(1)$ です。

# list

```cpp
std::vector<L> list()
```

全要素を昇順に返します。

計算量: $O(ノード数 + 要素数)$

# clear

```cpp
void clear()
```

全要素を削除します。

計算量: $O(ノード数)$

# copy / move

コピー後の集合は元の集合と独立です。copy constructorは $O(コピー元のノード数)$、copy assignmentは $O(コピー元と代入先のノード数の合計)$ です。move constructorは $O(1)$、move assignmentは代入先の旧ノード数に線形で、移動元は空集合になります。

## 時間計算量・空間計算量（公開API別）

各APIの時間計算量は直前の各節に示した通り。追加空間計算量は次の通りである。

- constructor、copy constructor、copy assignment、`clear`: 処理対象node数に比例（node本体または解放時の再帰stack）
- move constructor: $O(1)$、move assignment: 旧node解放の再帰stack $O(h)$
- `insert`、`erase`、`contains`、`count`、`range_sum`、`least`、`more`、`most`、`less`、`kth_ge`、`kth_gt`、`kth_le`、`kth_lt`、`min`、`max`: $O(h)$ の再帰stack
- `size`、`empty`: $O(1)$
- `list`: $O(h)$ のstackと、戻り値として $O(\mathtt{size()})$

## 注意点

`L` は `unsigned int` または `unsigned long long`、`0 < MAX_SIZE <= INT_MAX`。
`insert` / `erase` は `[0,MAX_SIZE)` の値を要求し、違反では `runtime_error`。
探索の範囲外は「存在しない」として扱い、`range_sum` は有効範囲へ切り詰める。
保存領域は生成node数を $K$ として $O(K)$。通常の更新・探索の再帰stackは $O(h)$、
`list` は戻り値の $O(size())$ に加えて $O(h)$ stackを使う。
