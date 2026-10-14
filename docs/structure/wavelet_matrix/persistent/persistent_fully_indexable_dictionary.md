---
title: Persistent Fully Indexable Dictionary (完全永続完備辞書)
documentation_of: ../../../../src/structure/wavelet_matrix/persistent/persistent_fully_indexable_dictionary.hpp
---

完全永続の完備辞書。任意のバージョンから分岐してbitを更新できる。

## 更新

```cpp
int nv = fid.set(version, k, value)
int nv = fid.flip(version, k)
int nv = fid.fork(version)
```

## クエリ

```cpp
fid.get(version, k)
fid.rank(version, value, r)
fid.rank(version, value, l, r)
fid.select(version, value, k)
```

## 時間計算量

- 更新: $O(\log (MAX\_SIZE / 64))$
- `rank`: $O(\log (MAX\_SIZE / 64))$
- `select`: `rank` に追加で二分探索

## 計算量（公開操作別）

$W=\lceil\mathtt{MAX\_SIZE}/64\rceil$、$H=\lceil\log_2\max(1,W)\rceil$ とする。

- int constructor: 固定容量のversion/root/node配列と初期木を含めて $O(W+\mathtt{MAX\_VERSION}(H+1))$
- vector constructor: $O(W+\mathtt{MAX\_VERSION}(H+1)+N)$
- `size`, `versions`, `latest_version`, `fork`: $O(1)$
- `get`, `set`, `flip`, `rank`: $O(H+1)$
- `select`: $O((H+1)\log N)$

`set`と`flip`は根から葉まで $O(H+1)$ 個のnodeを複製し、`fork`は根だけを共有する。

## 注意点

```cpp
int size() const
int versions() const
int latest_version() const
bool get(int version, int k) const
int set(int version, int k, bool value)
int flip(int version, int k)
int fork(int version)
int rank(int version, bool value, int r) const
int rank(int version, bool value, int l, int r) const
int select(int version, bool value, int k) const
```

更新と `fork` は指定versionから新versionを作り、その番号を返す。`rank` は半開区間、
`select` は0-indexedの出現位置で、該当なしなら `size()`。
不正なversion・点・区間・負の出現番号、`MAX_VERSION`またはnode容量超過では
`runtime_error`。失敗時にversion/node使用量は増えない。各APIの計算量は上記表の通り。

## Constructor signature

```cpp
PersistentFullyIndexableDictionary(int n = 0)
PersistentFullyIndexableDictionary(const vector<bool>& values)
```

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE`, `MAX_VERSION` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = PersistentFullyIndexableDictionary<>;
using Fixed = PersistentFullyIndexableDictionary<N, Versions>;
```

可変長版は必要なノード・版・履歴を明示的に追加し、保存要素数を $K$ として追加領域は $O(K)$。
再確保の費用は追加操作全体で償却され、上記各操作の計算量を維持する。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
