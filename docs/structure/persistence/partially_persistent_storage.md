---
title: Partially Persistent Storage (部分永続ストレージ)
documentation_of: ../../../src/structure/persistence/partially_persistent_storage.hpp
---

固定個数の要素について、現在値の更新と過去のバージョンの参照を行う内部向けのストレージである。バージョン番号は利用側が単調非減少に割り当てる。

```cpp
template<class T, int SIZE = -1, int MAX_CHANGE = -1>
struct PartiallyPersistentStorage;

PartiallyPersistentStorage<T, SIZE, MAX_CHANGE> storage;
storage.initialize(index, value);
storage.write(index, version, next_value);
const T& old = storage.get(index, old_version);
```

`SIZE` は要素数の上限、`MAX_CHANGE` は保持できる変更数の上限である。どちらも省略時（`-1`）は上限を設けず、実行時に `std::vector` で領域を確保する。

## API

$n$ を要素数、$C_k$ を添字 $k$ の変更回数、$R$ を取り消す変更数とする。

| 操作 | 引数・戻り値 | 前提・例外 | 時間計算量 |
| --- | --- | --- | --- |
| `PartiallyPersistentStorage(int n = SIZE)` | 要素数 $n$ のストレージを構築（`SIZE` 省略時のデフォルトは 0） | `0 <= n`、`SIZE` 指定時は `n <= SIZE`。違反時は `std::out_of_range` | $O(n)$（固定長版は $O(\texttt{SIZE}+\texttt{MAX_CHANGE})$） |
| `initialize(k, x)` | 添字 `k` の初期値を `x` にする | `0 <= k`、`SIZE` 指定時は `k < SIZE`。範囲外は `std::runtime_error`。`SIZE` 省略時は `k` まで領域を拡張する | $O(1)$（拡張時はならし） |
| `changes()` | 現在保持している変更数 | なし | $O(1)$ |
| `current(k)` | `k` の現在値への参照 | `k` が範囲内かつ初期化済み。実装は検査しない | $O(1)$ |
| `get(k, version)` | バージョン `version` 時点の値への参照 | `k` が範囲内かつ初期化済みで、同じ添字への `write` のバージョンが単調非減少。実装は検査しない | $O(\log C_k)$ |
| `write(k, version, x)` | `k` の変更履歴へ `(version, x)` を追加 | 添字の範囲とバージョンの単調性は実装が検査しない。変更数が `MAX_CHANGE` に達していれば `std::runtime_error` | ならし $O(1)$ |
| `rollback(snapshot)` | 変更数を `snapshot` まで戻す | `0 <= snapshot <= changes()`。範囲外は `std::runtime_error` | $O(R)$ |

値は `std::optional<T>` で保持し、初期値と変更値を合わせて $O(n+\texttt{MAX_CHANGE})$（`MAX_CHANGE` 省略時は変更数を $H$ として $O(n+H)$）の領域を使う。添字ごとの履歴を保持する `std::vector` の領域は、変更総数に比例する。

## 空間計算量（API 別の追加空間計算量）

- コンストラクタ: 保存領域の確保のみ
- `initialize`, `changes`, `current`, `get`, `rollback`: $O(1)$（`get` は反復による二分探索）
- `write`: $O(1)$ の履歴エントリを本体へ追加し、一時領域も $O(1)$

本体の保存領域は $O(\texttt{SIZE}+\texttt{MAX_CHANGE})$（容量省略時は $O(n+H)$）である。

## 注意点

- 内部実装向けである。各添字は `initialize` してから使い、同じ添字への `write` のバージョンは単調非減少でなければならない。この 2 条件は実装が検査しない。
- コピー・ムーブはできない（削除されている）。
- 容量を省略した場合、`get` / `current` が返す参照は、その後の `initialize` / `write` による領域拡張で無効になり得る。
