---
title: Partially Persistent Storage (部分永続ストレージ)
documentation_of: ../src/structure/other/partially_persistent_storage.hpp
---

固定個数の要素について、現在値の更新と過去versionの参照を行う内部向けstorage。
versionは利用側が単調非減少に割り当てる。

```cpp
PartiallyPersistentStorage<T, SIZE, MAX_CHANGE> storage;
storage.initialize(index, value);
storage.write(index, version, next_value);
const T& old = storage.get(index, old_version);
```

## API

| 操作 | 引数・戻り値 | 前提・例外 | 時間計算量 |
| --- | --- | --- | --- |
| `PartiallyPersistentStorage()` | 空のstorageを構築 | `SIZE > 0`, `MAX_CHANGE >= 0` | $O(SIZE+MAX\_CHANGE)$ |
| `initialize(k, x)` | index `k` の初期値を`x`にする | `0 <= k < SIZE`。範囲外は例外 | $O(1)$ |
| `changes()` | 現在保持する変更数 | なし | $O(1)$ |
| `current(k)` | `k`の現在値への参照 | `0 <= k < SIZE`かつ初期化済み。実装は検査しない | $O(1)$ |
| `get(k, version)` | `version`時点の値への参照 | `0 <= k < SIZE`、初期化済み、同じindexへのversionが単調非減少。実装は検査しない | $O(\log C_k)$ |
| `write(k, version, x)` | `k`の変更履歴へ`(version,x)`を追加 | index範囲とversion単調性は実装が検査しない。変更数が`MAX_CHANGE`なら例外 | amortized $O(1)$ |
| `rollback(snapshot)` | 変更数を`snapshot`まで戻す | `0 <= snapshot <= changes()`。範囲外は例外 | 取り消す変更数に対して$O(R)$ |

`C_k`はindex `k`の変更回数。値は`std::optional<T>`に保持され、初期値と変更値を合わせて固定容量$O(SIZE+MAX_CHANGE)$を使う。index別履歴のvector領域は変更総数$O(MAX_CHANGE)$。
