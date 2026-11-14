---
title: Dynamic Wavelet Matrix (動的ウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/dynamic/dynamic_wavelet_matrix.hpp
---

要素の挿入・削除・置換に対応した、整数列向けのウェーブレット行列。

各 bit 階層を B+ 木型の動的ビットベクトルで保持するため、列長が変わっても全体を再構築しない。次の操作を提供する。

- 要素の挿入・削除・置換
- `rank` / `select`
- 順序統計
- 値域頻度
- 前後探索
- 元値の総和
- 頻度列挙
- XOR / OR / AND で変換した値に対する検索

## テンプレート引数

```cpp
DynamicWaveletMatrix<
    T,
    MAX_SIZE = -1,
    BIT_WIDTH = (T の bit 数),
    SUM_TYPE = (自動選択),
    LEAF_WORDS = 2
>
```

| 引数 | 説明 |
|---|---|
| `T` | 要素型。`bool` 以外の整数型 |
| `MAX_SIZE` | 列長の上限。既定値 `-1` は上限なし（`int` の範囲内）。内部は B+ 木なので、指定しても確保する領域は変わらない |
| `BIT_WIDTH` | 値に使う bit 数。既定値は `std::numeric_limits<std::make_unsigned_t<T>>::digits` |
| `SUM_TYPE` | 総和 API の戻り値型。既定では `T` と `MAX_SIZE` から安全な符号付き型を選ぶ |
| `LEAF_WORDS` | 動的ビットベクトルの 1 葉に保持する 64 bit ワードの数。既定値は 2。葉の容量は `64 * LEAF_WORDS` bit |

符号付きの `T` では、`BIT_WIDTH` は型の全 bit 幅でなければならない。狭い `BIT_WIDTH` は符号なし型でのみ使える。

`SUM_TYPE` は次を満たす必要がある。

- `std::numeric_limits<SUM_TYPE>` が特殊化されている
- 有界な型なら符号付きである
- `MAX_SIZE` 個（`MAX_SIZE = -1` なら $2^{31}$ 個）の `T` の総和を表現できるだけの桁数を持つ

既定の `SUM_TYPE` は、安全なら `long long`、それで不足し処理系が `__int128_t` を提供する場合は `__int128_t` となる。安全な型を選べない場合はコンパイルエラーとなる。

## 公開型・定数

```cpp
using value_type = T;
using sum_type = SUM_TYPE;
using unsigned_value_type = std::make_unsigned_t<T>;

static constexpr bool has_sum_support = true;
static constexpr bool has_bitwise_support = true;

enum class BitwiseOperation {
    Xor,
    Or,
    And,
};
```

## 記号

- $N$: 現在の列長
- $N_{\mathrm{old}}$: 代入前の列長
- $W$: `BIT_WIDTH`
- $\sigma$: 列全体に現れる相異なる元の値の個数
- $C = 64 \cdot \texttt{LEAF_WORDS}$: 動的ビットベクトルの葉の容量
- $P(N) = O(\log N + \texttt{LEAF_WORDS})$: 動的ビットベクトル 1 階層での `rank` / `select` 相当の処理
- $S(N) = O(\log N + C)$: 動的ビットベクトル 1 階層での部分和付きの処理
- $U(N) = O(\log N + C)$: 動的ビットベクトル 1 階層での挿入・削除
- $m$: 元の値として列挙される相異なる値の個数
- $z$: bitwise 変換後に列挙される相異なる値の個数
- $q$: `intersect` で出力される共通の値の個数
- $V$: 列挙処理で訪問するウェーブレット行列上の状態数
- $M_{op}$: bitwise 検索で全階層を通して訪問する状態数

`LEAF_WORDS` を定数とみなせば、$P(N)$, $S(N)$, $U(N)$ はいずれも $O(\log N)$ である。

XOR では各階層の状態数は高々 1 つなので $M_{op} = O(W)$ である。OR / AND では複数の状態へ分岐し、最悪の場合は指数的になる。

## 共通規則

- 添字と出現番号 `k` / `kth` は 0-indexed である。
- 列の区間は半開区間 $[l, r)$ である。
- 値域は、特記しない限り半開区間 $[\mathit{lower}, \mathit{upper})$ である。
- `select` 系は対象が存在しない場合 `-1` を返す。
- `min_value`、`max_value`、中央値、前後探索、`mode` は対象が存在しない場合 `std::nullopt` を返す。
- `kth_smallest` / `kth_largest` / `quantile` は値を返す。内部要素への参照は返さない。
- `values()` もコピーを返すため、後続の `insert` / `erase` / `set` で戻り値が無効になることはない。

## 構築・コピー・再構築

| API | 説明 | 時間計算量 |
|---|---|---|
| `DynamicWaveletMatrix() = default` | 空列を構築する。各ビット階層の空葉を初期化する。 | $O(WC)$ |
| `explicit DynamicWaveletMatrix(const std::vector<T>& sequence)` | `sequence` から構築する。長さが `MAX_SIZE` を超える場合、または値が `BIT_WIDTH` に収まらない場合は `runtime_error`。 | $O(WN+N\log\sigma)$ |
| `template<std::size_t N> explicit DynamicWaveletMatrix(const std::array<T, N>& sequence)` | `std::array` から構築する。 | $O(WN+N\log\sigma)$ |
| `DynamicWaveletMatrix(const DynamicWaveletMatrix&) = default` | 深いコピーを行う。 | $O(WN+\sigma)$ |
| `DynamicWaveletMatrix& operator=(const DynamicWaveletMatrix&) = default` | 深いコピーで代入する。旧内容の破棄を含む。 | $O(W(N+N_{old})+\sigma+\sigma_{old})$ |
| `DynamicWaveletMatrix(DynamicWaveletMatrix&&) noexcept = default` | ムーブ構築する。 | $O(W)$ |
| `DynamicWaveletMatrix& operator=(DynamicWaveletMatrix&&) noexcept = default` | ムーブ代入する。旧内容の破棄を含む。 | $O(WN_{old}+\sigma_{old}+W)$ |
| `void build(const std::vector<T>& data)` | 既存内容を破棄し、`data` から再構築する。 | $O(W(N+N_{old})+N\log\sigma+\sigma_{old})$ |


デストラクタは保持する B+ 木、配列、頻度表を破棄するため、概ね $O(WN + \sigma)$ かかる。

## 基本操作・メタ情報・動的更新

| API | 説明 | 時間計算量 |
|---|---|---|
| `int size() const` | 現在の列長 `N` を返す。 | $O(1)$ |
| `bool empty() const` | 列が空なら `true`。 | $O(1)$ |
| `int distinct_size() const` | 列全体に現れる相異なる元の値の個数 $\sigma$ を返す。 | $O(1)$ |
| `int bit_size() const` | 空列なら `0`、それ以外は `BIT_WIDTH` を返す。 | $O(1)$ |
| `std::vector<T> values() const` | 列全体に現れる異なる元値を昇順で返す。戻り値はコピー。 | $O(\sigma)$ |
| `bool contains(const T& value) const` | 列全体に `value` が存在するかを返す。 | $O(\log\sigma)$ |
| `int index_of(const T& value) const` | `values()` が返す昇順列における `value` の添字。存在しなければ `-1`。 | 通常 $O(\log\sigma)$、更新後の初回は $O(\sigma)$ |
| `T access(int k) const` | `a[k]` を値で返す。 | $O(WP(N))$ |
| `T operator[](int k) const` | `access(k)` の別名。 | $O(WP(N))$ |
| `void insert(int k, T value)` | `a[k]` の直前へ `value` を挿入する。`k == N` なら末尾へ挿入する。 | $O(WU(N)+\log\sigma)$ |
| `T erase(int k)` | `a[k]` を削除し、削除した値を返す。 | $O(WU(N)+\log\sigma)$ |
| `void set(int k, T value)` | `a[k] = value` に点更新する。列長は変化しない。 | $O(WU(N)+\log\sigma)$ |


`values()` は常に戻り値のコピーに $O(\sigma)$ かかる。`index_of()` は動的更新後の最初の呼び出しで内部キャッシュを再構築するため $O(\sigma)$、キャッシュが有効な間は $O(\log \sigma)$ である。

## 出現回数・順序統計・範囲カウント・前後探索

| API | 説明 | 時間計算量 |
|---|---|---|
| `int rank(T value, int r) const` | 区間 `[0,r)` の `value` の個数。 | $O(WP(N))$ |
| `int rank(T value, int l, int r) const` | 区間 `[l,r)` の `value` の個数。 | $O(WP(N))$ |
| `int select(T value, int k) const` | 列全体で `value` が `k` 回目に現れる位置を返す。`k` は 0-indexed。存在しなければ `-1`。 | $O(WP(N))$ |
| `T kth_smallest(int l, int r, int k) const` | 区間 `[l,r)` の `k` 番目に小さい値を返す。 | $O(WP(N))$ |
| `T kth_largest(int l, int r, int k) const` | 区間 `[l,r)` の `k` 番目に大きい値を返す。 | $O(WP(N))$ |
| `int range_freq(int l, int r, T upper) const` | `x < upper` の要素数。 | $O(WP(N))$ |
| `int range_freq(int l, int r, T lower, T upper) const` | `lower <= x < upper` の要素数。 | $O(WP(N))$ |
| `std::optional<T> prev_value(int l, int r, T upper) const` | 区間内の `upper` 未満の最大値。存在しなければ `nullopt`。 | $O(WP(N))$ |
| `std::optional<T> next_value(int l, int r, T lower) const` | 区間内の `lower` 以上の最小値。存在しなければ `nullopt`。 | $O(WP(N))$ |
| `int count(const T& value) const` | 列全体の `value` の個数。 | $O(WP(N))$ |
| `int count(const T& value, int l, int r) const` | 区間 `[l,r)` の `value` の個数。 | $O(WP(N))$ |
| `T quantile(int l, int r, int k) const` | `kth_smallest` の別名。 | $O(WP(N))$ |
| `std::optional<T> min_value(int l, int r) const` | 区間の最小値。空区間なら `nullopt`。 | $O(WP(N))$ |
| `std::optional<T> max_value(int l, int r) const` | 区間の最大値。空区間なら `nullopt`。 | $O(WP(N))$ |
| `std::optional<T> median_lower(int l, int r) const` | 下側中央値、すなわち `(r-l-1)/2` 番目の値。空区間なら `nullopt`。 | $O(WP(N))$ |
| `std::optional<T> median_upper(int l, int r) const` | 上側中央値、すなわち `(r-l)/2` 番目の値。空区間なら `nullopt`。 | $O(WP(N))$ |
| `int count_less(int l, int r, const T& upper) const` | `x < upper` を満たす要素数。 | $O(WP(N))$ |
| `int count_less_equal(int l, int r, const T& upper) const` | `x <= upper` を満たす要素数。 | $O(WP(N))$ |
| `int count_greater(int l, int r, const T& lower) const` | `x > lower` を満たす要素数。 | $O(WP(N))$ |
| `int count_greater_equal(int l, int r, const T& lower) const` | `x >= lower` を満たす要素数。 | $O(WP(N))$ |
| `std::optional<T> next_value_ge(int l, int r, const T& lower) const` | `lower` 以上の最小値。`next_value` の別名。 | $O(WP(N))$ |
| `std::optional<T> next_value_gt(int l, int r, const T& lower) const` | `lower` より大きい最小値。 | $O(WP(N))$ |
| `std::optional<T> prev_value_lt(int l, int r, const T& upper) const` | `upper` 未満の最大値。`prev_value` の別名。 | $O(WP(N))$ |
| `std::optional<T> prev_value_le(int l, int r, const T& upper) const` | `upper` 以下の最大値。 | $O(WP(N))$ |


`kth_smallest`、`kth_largest`、`quantile` の `k` は `0 <= k < r-l` を満たす必要がある。

## 総和クエリ

総和はすべて、元の `T` の値を `sum_type` へ変換して加算する。bitwise API でも、条件の判定は変換後の値で行うが、加算の対象は元の値である。

| API | 説明 | 時間計算量 |
|---|---|---|
| `std::pair<int, sum_type> count_and_sum_less( int l, int r, const T& upper ) const` | `x < upper` の `(個数, 元値の総和)`。 | $O(WS(N))$ |
| `std::pair<int, sum_type> count_and_sum_less_equal( int l, int r, const T& upper ) const` | `x <= upper` の `(個数, 元値の総和)`。 | $O(WS(N))$ |
| `std::pair<int, sum_type> count_and_sum_greater( int l, int r, const T& lower ) const` | `x > lower` の `(個数, 元値の総和)`。 | $O(WS(N))$ |
| `std::pair<int, sum_type> count_and_sum_greater_equal( int l, int r, const T& lower ) const` | `x >= lower` の `(個数, 元値の総和)`。 | $O(WS(N))$ |
| `sum_type sum_all(int l, int r) const` | 区間内の全要素の総和。 | $O(S(N))$ |
| `sum_type sum_less(int l, int r, const T& upper) const` | `x < upper` の元値の総和。 | $O(WS(N))$ |
| `sum_type sum_less_equal(int l, int r, const T& upper) const` | `x <= upper` の元値の総和。 | $O(WS(N))$ |
| `sum_type sum_range(int l, int r, const T& lower, const T& upper) const` | `lower <= x < upper` の元値の総和。 | $O(WS(N))$ |
| `sum_type sum_equal(int l, int r, const T& value) const` | `x == value` の元値の総和。 | $O(WP(N))$ |
| `sum_type sum_greater(int l, int r, const T& lower) const` | `x > lower` の元値の総和。 | $O(WS(N))$ |
| `sum_type sum_greater_equal(int l, int r, const T& lower) const` | `x >= lower` の元値の総和。 | $O(WS(N))$ |
| `sum_type sum_k_smallest(int l, int r, int k) const` | 区間内の小さい方から `k` 個の総和。`k` は個数で、`0 <= k <= r-l`。 | $O(WS(N))$ |
| `sum_type sum_k_largest(int l, int r, int k) const` | 区間内の大きい方から `k` 個の総和。`k` は個数で、`0 <= k <= r-l`。 | $O(WS(N))$ |
| `int min_count_for_sum_smallest_ge(int l, int r, sum_type x) const` | 昇順に並べた先頭から総和を取り、初めて `x` 以上になる最小要素数。`x <= 0` なら `0`、達成不能なら `-1`。 | $O(WS(N))$ |
| `int min_count_for_sum_largest_ge(int l, int r, sum_type x) const` | 降順に並べた先頭から総和を取り、初めて `x` 以上になる最小要素数。`x <= 0` なら `0`、達成不能なら `-1`。 | $O(WS(N))$ |


### `min_count_for_sum_*_ge` の仕様

`min_count_for_sum_smallest_ge(l, r, x)` は、区間の値を昇順に並べたものを $b_0, b_1, \ldots$ としたとき、

$$
b_0 + \cdots + b_{k-1} \ge x
$$

を満たす最小の $k$ を返す。`min_count_for_sum_largest_ge` は降順について同様に求める。

- $x \le 0$ のとき: 空集合の総和を 0 とし、`0` を返す
- 達成できない場合: `-1` を返す
- 負の値を含む列にも対応する

## 列挙・頻度分析

| API | 説明 | 時間計算量 |
|---|---|---|
| `std::vector<std::pair<T, int>> list_frequencies(int l, int r) const` | 区間内の異なる元値を `(値, 頻度)` で昇順列挙する。 | $O(VP(N)+m)$ |
| `std::vector<std::pair<T, int>> list_frequencies( int l, int r, const T& lower, const T& upper ) const` | 区間内かつ値域 `[lower,upper)` の異なる元値を `(値, 頻度)` で昇順列挙する。 | $O(VP(N)+m)$ |
| `std::vector<T> distinct_values(int l, int r) const` | 区間内の異なる元値を昇順で返す。 | $O(VP(N)+m)$ |
| `std::vector<std::pair<T, int>> top_k_frequent(int l, int r, int k) const` | 区間内の頻度上位 `k` 個を `(値, 頻度)` で返す。頻度降順、同頻度では値昇順。`k <= 0` なら空。 | $O(V_k(P(N)+\log V_k)+k\log k)$ |
| `std::optional<std::pair<T, int>> mode(int l, int r) const` | 区間の最頻値を `(値, 頻度)` で返す。同頻度では最小値。空区間なら `nullopt`。 | $O(V_1(P(N)+\log V_1))$ |
| `std::vector<std::tuple<T, int, int>> intersect( int l1, int r1, int l2, int r2 ) const` | 2 区間に共通する異なる元値を `(値, 第1区間の頻度, 第2区間の頻度)` で昇順列挙する。 | $O(V_\cap P(N)+q)$ |


`top_k_frequent` はウェーブレット行列上の状態を、頻度順の優先度付きキューで探索する。$V_k$ は結果が `k` 個得られるまでに訪問する状態数である。`mode` は `top_k_frequent(..., 1)` に相当する。

## bitwise API の値の扱い

bitwise API では、元の値 `x` の `T` としての bit 表現に対して演算する。結果は `unsigned long long` で返し、値域は `BIT_WIDTH` bit の非負整数である。

```cpp
y = operation(x, mask)
```

- `mask` は下位 `BIT_WIDTH` bit のみ使う
- `Xor`: `y = x ^ mask`
- `Or`: `y = x | mask`
- `And`: `y = x & mask`
- 符号付きの `T` でも、結果の比較順序は変換後の符号なし値 `y` の順序である
- bitwise 総和 API が返す総和は、変換後の `y` ではなく元の `x` の総和である

## bitwise 共通 API

次の API は、`BitwiseOperation` を引数として XOR / OR / AND を共通に扱う。

| API | 説明 | 時間計算量 |
|---|---|---|
| `int count_less_bitwise( int l, int r, unsigned long long mask, unsigned long long upper, BitwiseOperation operation ) const` | 各要素に `operation(x,mask)` を施した値 `y` について、`y < upper` の個数。 | $O(M_{op}S(N))$ |
| `int range_freq_bitwise( int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper, BitwiseOperation operation ) const` | `lower <= y < upper` の個数。 | $O(M_{op}S(N))$ |
| `int count_less_equal_bitwise( int l, int r, unsigned long long mask, unsigned long long upper, BitwiseOperation operation ) const` | `y <= upper` の個数。 | $O(M_{op}S(N))$ |
| `int count_equal_bitwise( int l, int r, unsigned long long mask, unsigned long long value, BitwiseOperation operation ) const` | `y == value` の個数。 | $O(M_{op}S(N))$ |
| `int count_greater_bitwise( int l, int r, unsigned long long mask, unsigned long long lower, BitwiseOperation operation ) const` | `y > lower` の個数。 | $O(M_{op}S(N))$ |
| `int count_greater_equal_bitwise( int l, int r, unsigned long long mask, unsigned long long lower, BitwiseOperation operation ) const` | `y >= lower` の個数。 | $O(M_{op}S(N))$ |
| `std::pair<int, sum_type> count_and_sum_less_bitwise( int l, int r, unsigned long long mask, unsigned long long upper, BitwiseOperation operation ) const` | `y < upper` の `(個数, 元の x の総和)`。 | $O(M_{op}S(N))$ |
| `std::pair<int, sum_type> count_and_sum_less_equal_bitwise( int l, int r, unsigned long long mask, unsigned long long upper, BitwiseOperation operation ) const` | `y <= upper` の `(個数, 元の x の総和)`。 | $O(M_{op}S(N))$ |
| `std::pair<int, sum_type> count_and_sum_range_bitwise( int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper, BitwiseOperation operation ) const` | `lower <= y < upper` の `(個数, 元の x の総和)`。 | $O(M_{op}S(N))$ |
| `std::pair<int, sum_type> count_and_sum_greater_bitwise( int l, int r, unsigned long long mask, unsigned long long lower, BitwiseOperation operation ) const` | `y > lower` の `(個数, 元の x の総和)`。 | $O(M_{op}S(N))$ |
| `std::pair<int, sum_type> count_and_sum_greater_equal_bitwise( int l, int r, unsigned long long mask, unsigned long long lower, BitwiseOperation operation ) const` | `y >= lower` の `(個数, 元の x の総和)`。 | $O(M_{op}S(N))$ |
| `sum_type sum_less_bitwise( int l, int r, unsigned long long mask, unsigned long long upper, BitwiseOperation operation ) const` | `y < upper` を満たす元の `x` の総和。 | $O(M_{op}S(N))$ |
| `sum_type sum_less_equal_bitwise( int l, int r, unsigned long long mask, unsigned long long upper, BitwiseOperation operation ) const` | `y <= upper` を満たす元の `x` の総和。 | $O(M_{op}S(N))$ |
| `sum_type sum_equal_bitwise( int l, int r, unsigned long long mask, unsigned long long value, BitwiseOperation operation ) const` | `y == value` を満たす元の `x` の総和。 | $O(M_{op}S(N))$ |
| `sum_type sum_range_bitwise( int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper, BitwiseOperation operation ) const` | `lower <= y < upper` を満たす元の `x` の総和。 | $O(M_{op}S(N))$ |
| `sum_type sum_greater_bitwise( int l, int r, unsigned long long mask, unsigned long long lower, BitwiseOperation operation ) const` | `y > lower` を満たす元の `x` の総和。 | $O(M_{op}S(N))$ |
| `sum_type sum_greater_equal_bitwise( int l, int r, unsigned long long mask, unsigned long long lower, BitwiseOperation operation ) const` | `y >= lower` を満たす元の `x` の総和。 | $O(M_{op}S(N))$ |
| `unsigned long long access_bitwise( int i, unsigned long long mask, BitwiseOperation operation ) const` | `i` 番目の変換後の値 `y` を返す。 | $O(WP(N))$ |
| `int rank_bitwise( unsigned long long value, int r, unsigned long long mask, BitwiseOperation operation ) const` | 区間 `[0,r)` で変換後の値が `value` である個数。 | $O(M_{op}S(N))$ |
| `int rank_bitwise( unsigned long long value, int l, int r, unsigned long long mask, BitwiseOperation operation ) const` | 区間 `[l,r)` で変換後の値が `value` である個数。 | $O(M_{op}S(N))$ |
| `int count_bitwise( unsigned long long value, unsigned long long mask, BitwiseOperation operation ) const` | 列全体で変換後の値が `value` である個数。 | $O(M_{op}S(N))$ |
| `int count_bitwise( unsigned long long value, int l, int r, unsigned long long mask, BitwiseOperation operation ) const` | 区間 `[l,r)` で変換後の値が `value` である個数。 | $O(M_{op}S(N))$ |
| `bool contains_bitwise( unsigned long long value, unsigned long long mask, BitwiseOperation operation ) const` | 列全体に変換後の値 `value` が存在するか。 | $O(M_{op}S(N))$ |
| `int select_bitwise( unsigned long long value, int kth, unsigned long long mask, BitwiseOperation operation ) const` | 変換後の値 `value` の `kth` 回目の出現位置。存在しなければ `-1`。 | $O(M_{op}S(N)\log N)$ |
| `unsigned long long kth_smallest_bitwise( int l, int r, int k, unsigned long long mask, BitwiseOperation operation ) const` | 区間内の変換後の値の `k` 番目に小さい値。 | $O(M_{op}P(N))$ |
| `unsigned long long kth_largest_bitwise( int l, int r, int k, unsigned long long mask, BitwiseOperation operation ) const` | 区間内の変換後の値の `k` 番目に大きい値。 | $O(M_{op}P(N))$ |
| `unsigned long long quantile_bitwise( int l, int r, int k, unsigned long long mask, BitwiseOperation operation ) const` | `kth_smallest_bitwise` の別名。 | $O(M_{op}P(N))$ |
| `std::optional<unsigned long long> min_value_bitwise( int l, int r, unsigned long long mask, BitwiseOperation operation ) const` | 変換後の最小値。空区間なら `nullopt`。 | $O(M_{op}P(N))$ |
| `std::optional<unsigned long long> max_value_bitwise( int l, int r, unsigned long long mask, BitwiseOperation operation ) const` | 変換後の最大値。空区間なら `nullopt`。 | $O(M_{op}P(N))$ |
| `std::optional<unsigned long long> median_lower_bitwise( int l, int r, unsigned long long mask, BitwiseOperation operation ) const` | 変換後の下側中央値。空区間なら `nullopt`。 | $O(M_{op}P(N))$ |
| `std::optional<unsigned long long> median_upper_bitwise( int l, int r, unsigned long long mask, BitwiseOperation operation ) const` | 変換後の上側中央値。空区間なら `nullopt`。 | $O(M_{op}P(N))$ |
| `std::optional<unsigned long long> next_value_ge_bitwise( int l, int r, unsigned long long lower, unsigned long long mask, BitwiseOperation operation ) const` | 変換後の値のうち `lower` 以上の最小値。 | $O(M_{op}S(N))$ |
| `std::optional<unsigned long long> next_value_gt_bitwise( int l, int r, unsigned long long lower, unsigned long long mask, BitwiseOperation operation ) const` | 変換後の値のうち `lower` より大きい最小値。 | $O(M_{op}S(N))$ |
| `std::optional<unsigned long long> prev_value_lt_bitwise( int l, int r, unsigned long long upper, unsigned long long mask, BitwiseOperation operation ) const` | 変換後の値のうち `upper` 未満の最大値。 | $O(M_{op}S(N))$ |
| `std::optional<unsigned long long> prev_value_le_bitwise( int l, int r, unsigned long long upper, unsigned long long mask, BitwiseOperation operation ) const` | 変換後の値のうち `upper` 以下の最大値。 | $O(M_{op}S(N))$ |
| `std::optional<unsigned long long> next_value_bitwise( int l, int r, unsigned long long lower, unsigned long long mask, BitwiseOperation operation ) const` | `next_value_ge_bitwise` の別名。 | $O(M_{op}S(N))$ |
| `std::optional<unsigned long long> prev_value_bitwise( int l, int r, unsigned long long upper, unsigned long long mask, BitwiseOperation operation ) const` | `prev_value_lt_bitwise` の別名。 | $O(M_{op}S(N))$ |
| `std::vector<std::pair<unsigned long long, int>> list_frequencies_bitwise( int l, int r, unsigned long long mask, BitwiseOperation operation ) const` | 区間内の変換後の異なる値を `(値, 頻度)` で昇順列挙する。 | $O(VP(N)+m\log z+z)$ |
| `std::vector<std::pair<unsigned long long, int>> list_frequencies_bitwise( int l, int r, unsigned long long lower, unsigned long long upper, unsigned long long mask, BitwiseOperation operation ) const` | 区間内かつ変換後値域 `[lower,upper)` の異なる値を `(値, 頻度)` で昇順列挙する。 | $O(VP(N)+m\log z+z)$ |
| `std::vector<unsigned long long> distinct_values_bitwise( int l, int r, unsigned long long mask, BitwiseOperation operation ) const` | 区間内の変換後の異なる値を昇順で返す。 | $O(VP(N)+m\log z+z)$ |
| `std::vector<unsigned long long> values_bitwise( unsigned long long mask, BitwiseOperation operation ) const` | 列全体の変換後の異なる値を昇順で返す。 | $O(VP(N)+m\log z+z)$ |
| `int distinct_size_bitwise( unsigned long long mask, BitwiseOperation operation ) const` | 列全体の変換後の異なる値数。 | $O(VP(N)+m\log z+z)$ |
| `int index_of_bitwise( unsigned long long value, unsigned long long mask, BitwiseOperation operation ) const` | `values_bitwise` における `value` の位置。存在しなければ `-1`。 | $O(VP(N)+m\log z+z)$ |
| `std::vector<std::pair<unsigned long long, int>> top_k_frequent_bitwise( int l, int r, int k, unsigned long long mask, BitwiseOperation operation ) const` | 変換後の頻度上位 `k` 個。頻度降順、同頻度では値昇順。 | $O(VP(N)+m\log z+z\log z)$ |
| `std::optional<std::pair<unsigned long long, int>> mode_bitwise( int l, int r, unsigned long long mask, BitwiseOperation operation ) const` | 変換後の最頻値 `(値, 頻度)`。空区間なら `nullopt`。 | $O(VP(N)+m\log z+z\log z)$ |
| `std::vector<std::tuple<unsigned long long, int, int>> intersect_bitwise( int l1, int r1, int l2, int r2, unsigned long long mask, BitwiseOperation operation ) const` | 2 区間に共通する変換後の値を `(値, 第1頻度, 第2頻度)` で昇順列挙する。 | $O(V_1P(N)+V_2P(N)+m_1\log z_1+m_2\log z_2+z_1+z_2)$ |


$M_{op}$ は探索で訪問する状態の総数で、XOR では $O(W)$、OR / AND では最悪の場合に指数的である。`select_bitwise` は `rank_bitwise` を用いた位置の二分探索である。

列挙系の bitwise API は、元の値を列挙してから変換後に等しくなる値を集約する。このため、計算量の $m$ は変換前の相異なる値の個数、$z$ は変換後の相異なる値の個数である。

## XOR ラッパー

以下は `BitwiseOperation::Xor` に対応し、`y = x ^ mask` とする。個数、`rank` / `select`、順序統計、前後探索には XOR 専用の単一経路の実装を使う。

| API | 説明 | 時間計算量 |
|---|---|---|
| `int count_less_xor(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y < upper` の個数。 | $O(WP(N))$ |
| `int range_freq_xor(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const` | `lower <= y < upper` の個数。 | $O(WP(N))$ |
| `int count_less_equal_xor(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y <= upper` の個数。 | $O(WP(N))$ |
| `int count_equal_xor(int l, int r, unsigned long long mask, unsigned long long value) const` | `y == value` の個数。 | $O(WP(N))$ |
| `int count_greater_xor(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y > lower` の個数。 | $O(WP(N))$ |
| `int count_greater_equal_xor(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y >= lower` の個数。 | $O(WP(N))$ |
| `std::pair<int, sum_type> count_and_sum_less_xor(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y < upper` の `(個数, 元の x の総和)`。 | $O(WS(N))$ |
| `std::pair<int, sum_type> count_and_sum_less_equal_xor(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y <= upper` の `(個数, 元の x の総和)`。 | $O(WS(N))$ |
| `std::pair<int, sum_type> count_and_sum_range_xor(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const` | `lower <= y < upper` の `(個数, 元の x の総和)`。 | $O(WS(N))$ |
| `std::pair<int, sum_type> count_and_sum_greater_xor(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y > lower` の `(個数, 元の x の総和)`。 | $O(WS(N))$ |
| `std::pair<int, sum_type> count_and_sum_greater_equal_xor(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y >= lower` の `(個数, 元の x の総和)`。 | $O(WS(N))$ |
| `sum_type sum_less_xor(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y < upper` の元値の総和。 | $O(WS(N))$ |
| `sum_type sum_less_equal_xor(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y <= upper` の元値の総和。 | $O(WS(N))$ |
| `sum_type sum_equal_xor(int l, int r, unsigned long long mask, unsigned long long value) const` | `y == value` の元値の総和。 | $O(WS(N))$ |
| `sum_type sum_range_xor(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const` | `lower <= y < upper` の元値の総和。 | $O(WS(N))$ |
| `sum_type sum_greater_xor(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y > lower` の元値の総和。 | $O(WS(N))$ |
| `sum_type sum_greater_equal_xor(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y >= lower` の元値の総和。 | $O(WS(N))$ |
| `unsigned long long access_xor(int i, unsigned long long mask) const` | `i` 番目の変換後の値。 | $O(WP(N))$ |
| `int rank_xor(unsigned long long value, int r, unsigned long long mask) const` | 区間 `[0,r)` における変換後値 `value` の個数。 | $O(WP(N))$ |
| `int rank_xor(unsigned long long value, int l, int r, unsigned long long mask) const` | 区間 `[l,r)` における変換後値 `value` の個数。 | $O(WP(N))$ |
| `int count_xor(unsigned long long value, unsigned long long mask) const` | 列全体における変換後値 `value` の個数。 | $O(WP(N))$ |
| `int count_xor(unsigned long long value, int l, int r, unsigned long long mask) const` | 区間 `[l,r)` における変換後値 `value` の個数。 | $O(WP(N))$ |
| `bool contains_xor(unsigned long long value, unsigned long long mask) const` | 変換後の値 `value` が列全体に存在するか。 | $O(WP(N))$ |
| `std::vector<unsigned long long> values_xor(unsigned long long mask) const` | 列全体の変換後の異なる値を昇順で返す。 | $O(VP(N)+m\log m)$ |
| `int distinct_size_xor(unsigned long long mask) const` | 列全体の変換後の異なる値数。 | $O(1)$ |
| `int index_of_xor(unsigned long long value, unsigned long long mask) const` | 変換後の異なる値一覧における `value` の位置。存在しなければ `-1`。 | $O(VP(N)+m\log m)$ |
| `int select_xor(unsigned long long value, int kth, unsigned long long mask) const` | 変換後の値 `value` の `kth` 回目の位置。存在しなければ `-1`。 | $O(WP(N))$ |
| `unsigned long long kth_smallest_xor(int l, int r, int k, unsigned long long mask) const` | 変換後の値の `k` 番目に小さい値。 | $O(WP(N))$ |
| `unsigned long long kth_largest_xor(int l, int r, int k, unsigned long long mask) const` | 変換後の値の `k` 番目に大きい値。 | $O(WP(N))$ |
| `unsigned long long quantile_xor(int l, int r, int k, unsigned long long mask) const` | `kth_smallest` の別名。 | $O(WP(N))$ |
| `std::optional<unsigned long long> min_value_xor(int l, int r, unsigned long long mask) const` | 変換後の最小値。空区間なら `nullopt`。 | $O(WP(N))$ |
| `std::optional<unsigned long long> max_value_xor(int l, int r, unsigned long long mask) const` | 変換後の最大値。空区間なら `nullopt`。 | $O(WP(N))$ |
| `std::optional<unsigned long long> median_lower_xor(int l, int r, unsigned long long mask) const` | 変換後の下側中央値。空区間なら `nullopt`。 | $O(WP(N))$ |
| `std::optional<unsigned long long> median_upper_xor(int l, int r, unsigned long long mask) const` | 変換後の上側中央値。空区間なら `nullopt`。 | $O(WP(N))$ |
| `std::optional<unsigned long long> next_value_ge_xor(int l, int r, unsigned long long lower, unsigned long long mask) const` | 変換後の値のうち `lower` 以上の最小値。 | $O(WP(N))$ |
| `std::optional<unsigned long long> next_value_gt_xor(int l, int r, unsigned long long lower, unsigned long long mask) const` | 変換後の値のうち `lower` より大きい最小値。 | $O(WP(N))$ |
| `std::optional<unsigned long long> prev_value_lt_xor(int l, int r, unsigned long long upper, unsigned long long mask) const` | 変換後の値のうち `upper` 未満の最大値。 | $O(WP(N))$ |
| `std::optional<unsigned long long> prev_value_le_xor(int l, int r, unsigned long long upper, unsigned long long mask) const` | 変換後の値のうち `upper` 以下の最大値。 | $O(WP(N))$ |
| `std::optional<unsigned long long> next_value_xor(int l, int r, unsigned long long lower, unsigned long long mask) const` | `next_value_ge` の別名。 | $O(WP(N))$ |
| `std::optional<unsigned long long> prev_value_xor(int l, int r, unsigned long long upper, unsigned long long mask) const` | `prev_value_lt` の別名。 | $O(WP(N))$ |
| `std::vector<std::pair<unsigned long long, int>> list_frequencies_xor(int l, int r, unsigned long long mask) const` | 変換後の異なる値を `(値, 頻度)` で昇順列挙する。 | $O(VP(N)+m\log m)$ |
| `std::vector<std::pair<unsigned long long, int>> list_frequencies_xor(int l, int r, unsigned long long lower, unsigned long long upper, unsigned long long mask) const` | 区間内かつ変換後値域 `[lower,upper)` の異なる値を `(値, 頻度)` で昇順列挙する。 | $O(VP(N)+m\log m)$ |
| `std::vector<unsigned long long> distinct_values_xor(int l, int r, unsigned long long mask) const` | 区間内の変換後の異なる値を昇順で返す。 | $O(VP(N)+m\log m)$ |
| `std::vector<std::pair<unsigned long long, int>> top_k_frequent_xor(int l, int r, int k, unsigned long long mask) const` | 変換後の頻度上位 `k` 個。頻度降順、同頻度では値昇順。 | $O(VP(N)+m\log m)$ |
| `std::optional<std::pair<unsigned long long, int>> mode_xor(int l, int r, unsigned long long mask) const` | 変換後の最頻値 `(値, 頻度)`。空区間なら `nullopt`。 | $O(VP(N)+m\log m)$ |
| `std::vector<std::tuple<unsigned long long, int, int>> intersect_xor(int l1, int r1, int l2, int r2, unsigned long long mask) const` | 2 区間に共通する変換後の値を `(値, 第1頻度, 第2頻度)` で昇順列挙する。 | $O(V_1P(N)+V_2P(N)+m_1\log z_1+m_2\log z_2+z_1+z_2)$ |


`distinct_size_xor(mask)` は、XOR が全単射であるため `distinct_size()` と同じ値を $O(1)$ で返す。

## OR ラッパー

以下は `BitwiseOperation::Or` に対応し、`y = x | mask` とする。

| API | 説明 | 時間計算量 |
|---|---|---|
| `int count_less_or(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y < upper` の個数。 | $O(M_{op}S(N))$ |
| `int range_freq_or(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const` | `lower <= y < upper` の個数。 | $O(M_{op}S(N))$ |
| `int count_less_equal_or(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y <= upper` の個数。 | $O(M_{op}S(N))$ |
| `int count_equal_or(int l, int r, unsigned long long mask, unsigned long long value) const` | `y == value` の個数。 | $O(M_{op}S(N))$ |
| `int count_greater_or(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y > lower` の個数。 | $O(M_{op}S(N))$ |
| `int count_greater_equal_or(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y >= lower` の個数。 | $O(M_{op}S(N))$ |
| `std::pair<int, sum_type> count_and_sum_less_or(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y < upper` の `(個数, 元の x の総和)`。 | $O(M_{op}S(N))$ |
| `std::pair<int, sum_type> count_and_sum_less_equal_or(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y <= upper` の `(個数, 元の x の総和)`。 | $O(M_{op}S(N))$ |
| `std::pair<int, sum_type> count_and_sum_range_or(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const` | `lower <= y < upper` の `(個数, 元の x の総和)`。 | $O(M_{op}S(N))$ |
| `std::pair<int, sum_type> count_and_sum_greater_or(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y > lower` の `(個数, 元の x の総和)`。 | $O(M_{op}S(N))$ |
| `std::pair<int, sum_type> count_and_sum_greater_equal_or(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y >= lower` の `(個数, 元の x の総和)`。 | $O(M_{op}S(N))$ |
| `sum_type sum_less_or(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y < upper` の元値の総和。 | $O(M_{op}S(N))$ |
| `sum_type sum_less_equal_or(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y <= upper` の元値の総和。 | $O(M_{op}S(N))$ |
| `sum_type sum_equal_or(int l, int r, unsigned long long mask, unsigned long long value) const` | `y == value` の元値の総和。 | $O(M_{op}S(N))$ |
| `sum_type sum_range_or(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const` | `lower <= y < upper` の元値の総和。 | $O(M_{op}S(N))$ |
| `sum_type sum_greater_or(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y > lower` の元値の総和。 | $O(M_{op}S(N))$ |
| `sum_type sum_greater_equal_or(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y >= lower` の元値の総和。 | $O(M_{op}S(N))$ |
| `unsigned long long access_or(int i, unsigned long long mask) const` | `i` 番目の変換後の値。 | $O(WP(N))$ |
| `int rank_or(unsigned long long value, int r, unsigned long long mask) const` | 区間 `[0,r)` における変換後値 `value` の個数。 | $O(M_{op}S(N))$ |
| `int rank_or(unsigned long long value, int l, int r, unsigned long long mask) const` | 区間 `[l,r)` における変換後値 `value` の個数。 | $O(M_{op}S(N))$ |
| `int count_or(unsigned long long value, unsigned long long mask) const` | 列全体における変換後値 `value` の個数。 | $O(M_{op}S(N))$ |
| `int count_or(unsigned long long value, int l, int r, unsigned long long mask) const` | 区間 `[l,r)` における変換後値 `value` の個数。 | $O(M_{op}S(N))$ |
| `bool contains_or(unsigned long long value, unsigned long long mask) const` | 変換後の値 `value` が列全体に存在するか。 | $O(M_{op}S(N))$ |
| `std::vector<unsigned long long> values_or(unsigned long long mask) const` | 列全体の変換後の異なる値を昇順で返す。 | $O(VP(N)+m\log z+z)$ |
| `int distinct_size_or(unsigned long long mask) const` | 列全体の変換後の異なる値数。 | $O(VP(N)+m\log z+z)$ |
| `int index_of_or(unsigned long long value, unsigned long long mask) const` | 変換後の異なる値一覧における `value` の位置。存在しなければ `-1`。 | $O(VP(N)+m\log z+z)$ |
| `int select_or(unsigned long long value, int kth, unsigned long long mask) const` | 変換後の値 `value` の `kth` 回目の位置。存在しなければ `-1`。 | $O(M_{op}S(N)\log N)$ |
| `unsigned long long kth_smallest_or(int l, int r, int k, unsigned long long mask) const` | 変換後の値の `k` 番目に小さい値。 | $O(M_{op}P(N))$ |
| `unsigned long long kth_largest_or(int l, int r, int k, unsigned long long mask) const` | 変換後の値の `k` 番目に大きい値。 | $O(M_{op}P(N))$ |
| `unsigned long long quantile_or(int l, int r, int k, unsigned long long mask) const` | `kth_smallest` の別名。 | $O(M_{op}P(N))$ |
| `std::optional<unsigned long long> min_value_or(int l, int r, unsigned long long mask) const` | 変換後の最小値。空区間なら `nullopt`。 | $O(M_{op}P(N))$ |
| `std::optional<unsigned long long> max_value_or(int l, int r, unsigned long long mask) const` | 変換後の最大値。空区間なら `nullopt`。 | $O(M_{op}P(N))$ |
| `std::optional<unsigned long long> median_lower_or(int l, int r, unsigned long long mask) const` | 変換後の下側中央値。空区間なら `nullopt`。 | $O(M_{op}P(N))$ |
| `std::optional<unsigned long long> median_upper_or(int l, int r, unsigned long long mask) const` | 変換後の上側中央値。空区間なら `nullopt`。 | $O(M_{op}P(N))$ |
| `std::optional<unsigned long long> next_value_ge_or(int l, int r, unsigned long long lower, unsigned long long mask) const` | 変換後の値のうち `lower` 以上の最小値。 | $O(M_{op}S(N))$ |
| `std::optional<unsigned long long> next_value_gt_or(int l, int r, unsigned long long lower, unsigned long long mask) const` | 変換後の値のうち `lower` より大きい最小値。 | $O(M_{op}S(N))$ |
| `std::optional<unsigned long long> prev_value_lt_or(int l, int r, unsigned long long upper, unsigned long long mask) const` | 変換後の値のうち `upper` 未満の最大値。 | $O(M_{op}S(N))$ |
| `std::optional<unsigned long long> prev_value_le_or(int l, int r, unsigned long long upper, unsigned long long mask) const` | 変換後の値のうち `upper` 以下の最大値。 | $O(M_{op}S(N))$ |
| `std::optional<unsigned long long> next_value_or(int l, int r, unsigned long long lower, unsigned long long mask) const` | `next_value_ge` の別名。 | $O(M_{op}S(N))$ |
| `std::optional<unsigned long long> prev_value_or(int l, int r, unsigned long long upper, unsigned long long mask) const` | `prev_value_lt` の別名。 | $O(M_{op}S(N))$ |
| `std::vector<std::pair<unsigned long long, int>> list_frequencies_or(int l, int r, unsigned long long mask) const` | 変換後の異なる値を `(値, 頻度)` で昇順列挙する。 | $O(VP(N)+m\log z+z)$ |
| `std::vector<std::pair<unsigned long long, int>> list_frequencies_or(int l, int r, unsigned long long lower, unsigned long long upper, unsigned long long mask) const` | 区間内かつ変換後値域 `[lower,upper)` の異なる値を `(値, 頻度)` で昇順列挙する。 | $O(VP(N)+m\log z+z)$ |
| `std::vector<unsigned long long> distinct_values_or(int l, int r, unsigned long long mask) const` | 区間内の変換後の異なる値を昇順で返す。 | $O(VP(N)+m\log z+z)$ |
| `std::vector<std::pair<unsigned long long, int>> top_k_frequent_or(int l, int r, int k, unsigned long long mask) const` | 変換後の頻度上位 `k` 個。頻度降順、同頻度では値昇順。 | $O(VP(N)+m\log z+z\log z)$ |
| `std::optional<std::pair<unsigned long long, int>> mode_or(int l, int r, unsigned long long mask) const` | 変換後の最頻値 `(値, 頻度)`。空区間なら `nullopt`。 | $O(VP(N)+m\log z+z\log z)$ |
| `std::vector<std::tuple<unsigned long long, int, int>> intersect_or(int l1, int r1, int l2, int r2, unsigned long long mask) const` | 2 区間に共通する変換後の値を `(値, 第1頻度, 第2頻度)` で昇順列挙する。 | $O(V_1P(N)+V_2P(N)+m_1\log z_1+m_2\log z_2+z_1+z_2)$ |


## AND ラッパー

以下は `BitwiseOperation::And` に対応し、`y = x & mask` とする。

| API | 説明 | 時間計算量 |
|---|---|---|
| `int count_less_and(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y < upper` の個数。 | $O(M_{op}S(N))$ |
| `int range_freq_and(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const` | `lower <= y < upper` の個数。 | $O(M_{op}S(N))$ |
| `int count_less_equal_and(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y <= upper` の個数。 | $O(M_{op}S(N))$ |
| `int count_equal_and(int l, int r, unsigned long long mask, unsigned long long value) const` | `y == value` の個数。 | $O(M_{op}S(N))$ |
| `int count_greater_and(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y > lower` の個数。 | $O(M_{op}S(N))$ |
| `int count_greater_equal_and(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y >= lower` の個数。 | $O(M_{op}S(N))$ |
| `std::pair<int, sum_type> count_and_sum_less_and(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y < upper` の `(個数, 元の x の総和)`。 | $O(M_{op}S(N))$ |
| `std::pair<int, sum_type> count_and_sum_less_equal_and(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y <= upper` の `(個数, 元の x の総和)`。 | $O(M_{op}S(N))$ |
| `std::pair<int, sum_type> count_and_sum_range_and(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const` | `lower <= y < upper` の `(個数, 元の x の総和)`。 | $O(M_{op}S(N))$ |
| `std::pair<int, sum_type> count_and_sum_greater_and(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y > lower` の `(個数, 元の x の総和)`。 | $O(M_{op}S(N))$ |
| `std::pair<int, sum_type> count_and_sum_greater_equal_and(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y >= lower` の `(個数, 元の x の総和)`。 | $O(M_{op}S(N))$ |
| `sum_type sum_less_and(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y < upper` の元値の総和。 | $O(M_{op}S(N))$ |
| `sum_type sum_less_equal_and(int l, int r, unsigned long long mask, unsigned long long upper) const` | `y <= upper` の元値の総和。 | $O(M_{op}S(N))$ |
| `sum_type sum_equal_and(int l, int r, unsigned long long mask, unsigned long long value) const` | `y == value` の元値の総和。 | $O(M_{op}S(N))$ |
| `sum_type sum_range_and(int l, int r, unsigned long long mask, unsigned long long lower, unsigned long long upper) const` | `lower <= y < upper` の元値の総和。 | $O(M_{op}S(N))$ |
| `sum_type sum_greater_and(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y > lower` の元値の総和。 | $O(M_{op}S(N))$ |
| `sum_type sum_greater_equal_and(int l, int r, unsigned long long mask, unsigned long long lower) const` | `y >= lower` の元値の総和。 | $O(M_{op}S(N))$ |
| `unsigned long long access_and(int i, unsigned long long mask) const` | `i` 番目の変換後の値。 | $O(WP(N))$ |
| `int rank_and(unsigned long long value, int r, unsigned long long mask) const` | 区間 `[0,r)` における変換後値 `value` の個数。 | $O(M_{op}S(N))$ |
| `int rank_and(unsigned long long value, int l, int r, unsigned long long mask) const` | 区間 `[l,r)` における変換後値 `value` の個数。 | $O(M_{op}S(N))$ |
| `int count_and(unsigned long long value, unsigned long long mask) const` | 列全体における変換後値 `value` の個数。 | $O(M_{op}S(N))$ |
| `int count_and(unsigned long long value, int l, int r, unsigned long long mask) const` | 区間 `[l,r)` における変換後値 `value` の個数。 | $O(M_{op}S(N))$ |
| `bool contains_and(unsigned long long value, unsigned long long mask) const` | 変換後の値 `value` が列全体に存在するか。 | $O(M_{op}S(N))$ |
| `std::vector<unsigned long long> values_and(unsigned long long mask) const` | 列全体の変換後の異なる値を昇順で返す。 | $O(VP(N)+m\log z+z)$ |
| `int distinct_size_and(unsigned long long mask) const` | 列全体の変換後の異なる値数。 | $O(VP(N)+m\log z+z)$ |
| `int index_of_and(unsigned long long value, unsigned long long mask) const` | 変換後の異なる値一覧における `value` の位置。存在しなければ `-1`。 | $O(VP(N)+m\log z+z)$ |
| `int select_and(unsigned long long value, int kth, unsigned long long mask) const` | 変換後の値 `value` の `kth` 回目の位置。存在しなければ `-1`。 | $O(M_{op}S(N)\log N)$ |
| `unsigned long long kth_smallest_and(int l, int r, int k, unsigned long long mask) const` | 変換後の値の `k` 番目に小さい値。 | $O(M_{op}P(N))$ |
| `unsigned long long kth_largest_and(int l, int r, int k, unsigned long long mask) const` | 変換後の値の `k` 番目に大きい値。 | $O(M_{op}P(N))$ |
| `unsigned long long quantile_and(int l, int r, int k, unsigned long long mask) const` | `kth_smallest` の別名。 | $O(M_{op}P(N))$ |
| `std::optional<unsigned long long> min_value_and(int l, int r, unsigned long long mask) const` | 変換後の最小値。空区間なら `nullopt`。 | $O(M_{op}P(N))$ |
| `std::optional<unsigned long long> max_value_and(int l, int r, unsigned long long mask) const` | 変換後の最大値。空区間なら `nullopt`。 | $O(M_{op}P(N))$ |
| `std::optional<unsigned long long> median_lower_and(int l, int r, unsigned long long mask) const` | 変換後の下側中央値。空区間なら `nullopt`。 | $O(M_{op}P(N))$ |
| `std::optional<unsigned long long> median_upper_and(int l, int r, unsigned long long mask) const` | 変換後の上側中央値。空区間なら `nullopt`。 | $O(M_{op}P(N))$ |
| `std::optional<unsigned long long> next_value_ge_and(int l, int r, unsigned long long lower, unsigned long long mask) const` | 変換後の値のうち `lower` 以上の最小値。 | $O(M_{op}S(N))$ |
| `std::optional<unsigned long long> next_value_gt_and(int l, int r, unsigned long long lower, unsigned long long mask) const` | 変換後の値のうち `lower` より大きい最小値。 | $O(M_{op}S(N))$ |
| `std::optional<unsigned long long> prev_value_lt_and(int l, int r, unsigned long long upper, unsigned long long mask) const` | 変換後の値のうち `upper` 未満の最大値。 | $O(M_{op}S(N))$ |
| `std::optional<unsigned long long> prev_value_le_and(int l, int r, unsigned long long upper, unsigned long long mask) const` | 変換後の値のうち `upper` 以下の最大値。 | $O(M_{op}S(N))$ |
| `std::optional<unsigned long long> next_value_and(int l, int r, unsigned long long lower, unsigned long long mask) const` | `next_value_ge` の別名。 | $O(M_{op}S(N))$ |
| `std::optional<unsigned long long> prev_value_and(int l, int r, unsigned long long upper, unsigned long long mask) const` | `prev_value_lt` の別名。 | $O(M_{op}S(N))$ |
| `std::vector<std::pair<unsigned long long, int>> list_frequencies_and(int l, int r, unsigned long long mask) const` | 変換後の異なる値を `(値, 頻度)` で昇順列挙する。 | $O(VP(N)+m\log z+z)$ |
| `std::vector<std::pair<unsigned long long, int>> list_frequencies_and(int l, int r, unsigned long long lower, unsigned long long upper, unsigned long long mask) const` | 区間内かつ変換後値域 `[lower,upper)` の異なる値を `(値, 頻度)` で昇順列挙する。 | $O(VP(N)+m\log z+z)$ |
| `std::vector<unsigned long long> distinct_values_and(int l, int r, unsigned long long mask) const` | 区間内の変換後の異なる値を昇順で返す。 | $O(VP(N)+m\log z+z)$ |
| `std::vector<std::pair<unsigned long long, int>> top_k_frequent_and(int l, int r, int k, unsigned long long mask) const` | 変換後の頻度上位 `k` 個。頻度降順、同頻度では値昇順。 | $O(VP(N)+m\log z+z\log z)$ |
| `std::optional<std::pair<unsigned long long, int>> mode_and(int l, int r, unsigned long long mask) const` | 変換後の最頻値 `(値, 頻度)`。空区間なら `nullopt`。 | $O(VP(N)+m\log z+z\log z)$ |
| `std::vector<std::tuple<unsigned long long, int, int>> intersect_and(int l1, int r1, int l2, int r2, unsigned long long mask) const` | 2 区間に共通する変換後の値を `(値, 第1頻度, 第2頻度)` で昇順列挙する。 | $O(V_1P(N)+V_2P(N)+m_1\log z_1+m_2\log z_2+z_1+z_2)$ |


## SIMD・CPU 命令による最適化

### AVX-512F

構築時の各階層での安定分割では、次の条件をすべて満たす場合に AVX-512F の compress-store を使う。

- x86 / x86-64
- GCC または Clang
- 実行時に `__builtin_cpu_supports("avx512f")` が真
- 階層の要素数が 1024 以上
- `sizeof(unsigned_value_type)` が 4 または 8

対象関数には `target("avx512f")` 属性が付いているため、翻訳単位全体を `-mavx512f` でコンパイルする必要はない。条件を満たさない場合はスカラ実装にフォールバックする。

### BMI2

葉内の `select` では、x86 / x86-64 かつ GCC または Clang で、実行時に BMI2 対応が確認できる場合、`PDEP` を使って 64 bit ワード内の対象 bit の位置を求める。非対応環境ではスカラ実装にフォールバックする。

### AVX2

現在の実装に AVX2 専用の経路はない。小さな葉に対する popcount や移動には、スカラの `POPCNT`、ワードシフト、`std::memmove` を使う。

## 空間計算量

- 各要素は $W$ 階層の動的ビットベクトルに bit と重みを保持する。
- 列全体の相異なる値と頻度を平衡木（`std::map`）で保持する。

したがって全体は概ね $O(WN + \sigma)$ 要素分の領域を使う。各階層では 1 要素につき 1 bit に加えて総和計算用の `T` 型の重みを保持するため、bit のみのウェーブレット行列よりメモリ使用量が大きい。

## 注意点

### コンパイル時の制約

次の場合は `static_assert` に失敗する。

- `T` が整数型でない
- `T` が `bool`
- `MAX_SIZE` が `-1` 未満
- `BIT_WIDTH` が範囲外
- 符号付きの `T` で `BIT_WIDTH` が型の全 bit 幅でない
- `LEAF_WORDS <= 0`
- `SUM_TYPE` が総和を安全に表現できない

### エラー処理

次の場合は `std::runtime_error` を送出する。

- コンストラクタまたは `build` の列長が `MAX_SIZE` を超える
- 値が指定した `BIT_WIDTH` に収まらない
- `insert` の位置が $0 \le k \le N$ を満たさない、または列長が `MAX_SIZE`（`-1` なら `int` の最大値）に達している
- `erase` / `set` / `access` の添字が $0 \le k \lt N$ を満たさない
- `insert` / `set` の値が指定した `BIT_WIDTH` に収まらない
- 区間が $0 \le l \le r \le N$ を満たさない
- `kth_*` の `k` が範囲外
- `sum_k_*` の `k` が $0 \le k \le r - l$ を満たさない
- 値域を取る API で `upper < lower`

### 戻り値で失敗を表す API

- `select` / `select_bitwise` / `select_xor` / `select_or` / `select_and`: `-1`
- `top_k_frequent` 系で `k <= 0`: 空の `std::vector`
- 空区間の最小値・最大値・中央値・`mode`: `nullopt`
- 前後探索で該当する値がない: `nullopt`
- `min_count_for_sum_*_ge` で達成不能: `-1`

## 時間計算量一覧

`LEAF_WORDS` を定数として簡略化すると、代表的な操作は次のとおり。

| 操作 | 時間計算量 |
|---|---|
| コンストラクタによる構築 | $O(WN + N \log \sigma)$ |
| `build` | 新旧データを含め $O(W(N + N_{\mathrm{old}}) + N \log \sigma + \sigma_{\mathrm{old}})$ |
| `access` / `operator[]` | $O(W \log N)$ |
| `insert` / `erase` / `set` | $O(W \log N + \log \sigma)$ |
| 通常の `rank` / `select` / 順序統計 / 値域頻度 | $O(W \log N)$ |
| 通常の条件付き総和 | $O(W \log N)$ |
| `sum_all` | $O(\log N)$ |
| XOR の個数・順序統計 | $O(W \log N)$ |
| OR / AND の検索 | 訪問状態数に比例し、最悪の場合は指数的 |
| 通常の列挙 | 訪問状態数に比例する出力依存の計算量 |
