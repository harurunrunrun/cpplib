---
title: Partially Persistent Fully Indexable Dictionary (部分永続完備辞書)
documentation_of: ../../../../src/structure/wavelet_matrix/partially_persistent/partially_persistent_fully_indexable_dictionary.hpp
---

部分永続の完備辞書。更新は常に最新版に対して行い、過去の版は `get` / `rank` / `select` で参照できる。64 bit ワードを葉とする永続セグメント木で実装されている。

## テンプレート引数

```cpp
PartiallyPersistentFullyIndexableDictionary<MAX_SIZE = -1, MAX_VERSION = -1>
```

- `MAX_SIZE`: bit 列長の上限
- `MAX_VERSION`: 作成できる新しい版の数の上限（初期版を含めて `MAX_VERSION + 1` 版まで）

どちらかを省略（`-1`）すると、ノードや版の配列を必要に応じて `std::vector` で伸長する可変長版になる。保存要素数を $K$ とすると追加の領域は $O(K)$ で、再確保の費用は全体で償却される。両方を指定すると固定長配列版になり、ノード数の上限を事前に計算して確保する。

```cpp
using Variable = PartiallyPersistentFullyIndexableDictionary<>;
using Fixed = PartiallyPersistentFullyIndexableDictionary<N, Versions>;
```

## コンストラクタ

```cpp
explicit PartiallyPersistentFullyIndexableDictionary(int n = 0)
explicit PartiallyPersistentFullyIndexableDictionary(const vector<bool>& values)
```

長さ `n` の全 0 の bit 列、または `values` を版 0 として構築する。

## 更新

```cpp
int set(int k, bool value)
int flip(int k)
```

最新版をもとに新しい版を作り、その番号を返す。根から葉まで $O(H + 1)$ 個のノードを複製する。

## クエリ

```cpp
int size() const
int versions() const
int latest_version() const
bool get(int version, int k) const
int rank(int version, bool value, int r) const
int rank(int version, bool value, int l, int r) const
int select(int version, bool value, int k) const
```

任意の有効な版 `version` を読む。意味は Persistent Fully Indexable Dictionary と同じで、`rank` は半開区間、`select` は 0-indexed の出現番号を取り、該当がなければ `size()` を返す。

## 時間計算量

$N$ を bit 列長とし、$W$ を固定長版では $\lceil \texttt{MAX_SIZE} / 64 \rceil$、可変長版では $\lceil N / 64 \rceil$、$H = \lceil \log_2 \max(1, W) \rceil$ とする。

- コンストラクタ（`int`）: $O(W)$（固定長版ではノード配列の初期化に $O(\texttt{MAX_VERSION} \cdot (H + 1))$ が加わる）
- コンストラクタ（`vector<bool>`）: 上記に $O(N)$ が加わる
- `size`, `versions`, `latest_version`: $O(1)$
- `get`, `set`, `flip`, `rank`: $O(H + 1)$
- `select`: $O((H + 1) \log N)$

## 注意点

- 不正な版・添字・区間、負の出現番号、版数やノード数の上限超過では `std::runtime_error` を送出する。
