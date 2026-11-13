---
title: Integer Set (整数集合) [predecessor_problem]
documentation_of: ../../../../src/structure/ordered_set/set/integer_set.hpp
---

$[0, \texttt{MAX_SIZE})$ の整数を管理する集合である。各ノードを 16 分岐にした多分木で、ノード内に要素数の累積和を保持する。ノードは必要な部分だけ動的に作る。

```cpp
template<class L, L MAX_SIZE = static_cast<L>(std::numeric_limits<int>::max())>
struct Int_Set;
```

- `L` は `unsigned int` または `unsigned long long`
- `0 < MAX_SIZE <= std::numeric_limits<int>::max()`（`static_assert` で検査する）
- `MAX_SIZE` は配列の確保個数ではなく座標の上限であり、省略時は `INT_MAX`。全座標分の配列は確保しない。

x86 環境では実行時に AVX-512、AVX2 の順に利用可能かを判定し、どちらも利用できない場合はスカラー実装を使う。AVX を有効にするコンパイルオプションは不要である。

以下で $h = O(\log_{16} \texttt{MAX_SIZE})$ とする。`kth_*` の `k` は 0-indexed である。該当する要素が存在しない操作は `std::nullopt` を返す。

各メンバ関数には `L` を受け取るものに加えて、符号付き整数 `std::make_signed_t<L>` を受け取るオーバーロードがある。符号付き版では、負の値を範囲外として扱う（`insert` / `erase` は例外、探索系は範囲外の部分を「存在しない」として扱う）。

デフォルトコンストラクタとムーブコンストラクタは $O(1)$ である。

## insert

```cpp
void insert(L x)
```

`x` を挿入する。すでに存在する場合は何もしない。`x` が範囲外の場合は `std::runtime_error` を送出する。

時間計算量: $O(h)$

## erase

```cpp
void erase(L x)
```

`x` を削除する。存在しない場合は何もしない。`x` が範囲外の場合は `std::runtime_error` を送出する。

時間計算量: $O(h)$

## contain

```cpp
bool contain(L x)
```

`x` が存在するかを返す。範囲外では `false` を返す。

時間計算量: $O(h)$

## range_sum

```cpp
L range_sum(L l, L r)
```

`[l, r)` に含まれる要素数を返す。区間のうち $[0, \texttt{MAX_SIZE})$ の外の部分は切り詰められる。

時間計算量: $O(h)$

## least / more

```cpp
std::optional<L> least(L x)
std::optional<L> more(L x)
```

- `least(x)`: `x <= y` を満たす最小の要素 `y`
- `more(x)`: `x < y` を満たす最小の要素 `y`

時間計算量: $O(h)$

## most / less

```cpp
std::optional<L> most(L x)
std::optional<L> less(L x)
```

- `most(x)`: `y <= x` を満たす最大の要素 `y`
- `less(x)`: `y < x` を満たす最大の要素 `y`

時間計算量: $O(h)$

## kth_ge / kth_gt

```cpp
std::optional<L> kth_ge(L x, L k)
std::optional<L> kth_gt(L x, L k)
```

- `kth_ge(x, k)`: `x <= y` を満たす要素を昇順に並べたときの `k` 番目
- `kth_gt(x, k)`: `x < y` を満たす要素を昇順に並べたときの `k` 番目

時間計算量: $O(16h)$

## kth_le / kth_lt

```cpp
std::optional<L> kth_le(L x, L k)
std::optional<L> kth_lt(L x, L k)
```

- `kth_le(x, k)`: `y <= x` を満たす要素を降順に並べたときの `k` 番目
- `kth_lt(x, k)`: `y < x` を満たす要素を降順に並べたときの `k` 番目

時間計算量: $O(16h)$

`kth_*` には `x` と `k` の一方または両方が符号付きのオーバーロードもある。負の `k` に対しては `std::nullopt` を返す。

## min / max / empty

```cpp
std::optional<L> min()
std::optional<L> max()
bool empty()
```

最小要素、最大要素、集合が空かどうかを返す。時間計算量は `min` と `max` が $O(h)$、`empty` が $O(1)$ である。

## list

```cpp
std::vector<L> list()
```

全要素を昇順に並べて返す。

時間計算量: $O(K + n)$（$K$ はノード数、$n$ は要素数）

## clear

```cpp
void clear()
```

全要素を削除する。

時間計算量: $O(K)$（$K$ はノード数）

## コピー / ムーブ

コピー後の集合は元の集合と独立である。コピーコンストラクタはコピー元のノード数に線形、コピー代入はコピー元と代入先のノード数の合計に線形の時間がかかる。ムーブコンストラクタは $O(1)$、ムーブ代入は代入先の旧ノード数に線形で、移動元は空集合になる。

## 空間計算量

各 API の時間計算量は前の各節に示した通りである。追加の空間計算量は次の通り。

- コンストラクタ、コピーコンストラクタ、コピー代入、`clear`: 処理対象のノード数に比例（ノード本体、または解放時の再帰スタック）
- ムーブコンストラクタ: $O(1)$、ムーブ代入: 旧ノード解放の再帰スタック $O(h)$
- `insert`, `erase`, `contain`, `range_sum`, `least`, `more`, `most`, `less`, `kth_ge`, `kth_gt`, `kth_le`, `kth_lt`, `min`, `max`: $O(h)$ の再帰スタック
- `empty`: $O(1)$
- `list`: $O(h)$ のスタックと、戻り値の $O(n)$

保存領域は生成されたノード数を $K$ として $O(K)$ である。

## 注意点

- `L` は `unsigned int` または `unsigned long long`、`0 < MAX_SIZE <= INT_MAX` である。
- `insert` / `erase` は $[0, \texttt{MAX_SIZE})$ の値を要求し、範囲外では `std::runtime_error` を送出する。
- 探索系の関数では範囲外の値を「存在しない」として扱い、`range_sum` は有効範囲へ切り詰める。
- 要素数を返す `size` や `count` は無い。要素数は `range_sum(0, MAX_SIZE)` で求められる。
