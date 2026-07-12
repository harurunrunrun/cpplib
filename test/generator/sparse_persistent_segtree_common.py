# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import random
from pathlib import Path

SIZE = 1_000_000_000_039


def range_sum(values: dict[int, int], left: int, right: int) -> int:
    return sum(value for index, value in values.items() if left <= index < right)


def write(out_dir: Path, commands: list[str], answers: list[int]) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "case_00.in").write_text(str(len(commands)) + "\n" + "\n".join(commands) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(map(str, answers)) + "\n", encoding="utf-8")


def random_range(rng: random.Random) -> tuple[int, int]:
    left = rng.randrange(SIZE + 1)
    right = rng.randrange(left, SIZE + 1)
    return left, right


def generate_persistent(out_dir: Path, wide: bool) -> None:
    rng = random.Random(20260901 + wide)
    versions: list[dict[int, int]] = [{}]
    commands: list[str] = []
    answers: list[int] = []

    def update(base: int, index: int, value: int, add: bool = False) -> None:
        state = versions[base].copy()
        state[index] = state.get(index, 0) + value if add else value
        if state[index] == 0:
            state.pop(index)
        commands.append(f"{'D' if add else 'S'} {base} {index} {value}")
        versions.append(state)
        answers.append(len(versions) - 1)

    update(0, SIZE - 1, 5)
    update(0, 7, 3)
    update(1, 7, 4, wide)
    commands.extend(["A 0", "A 1", "A 2", "P 3 0 1000000000039", "G 1 7", "G 3 7"])
    answers.extend([0, 5, 3, sum(versions[3].values()), 0, versions[3].get(7, 0)])

    for _ in range(180):
        action = rng.randrange(100)
        version = rng.randrange(len(versions))
        if action < 48 and len(versions) < 250:
            index = rng.choice([0, SIZE - 1, rng.randrange(SIZE)])
            value = rng.randrange(-10**9, 10**9 + 1)
            update(version, index, value, wide and rng.randrange(2) == 0)
        elif action < 58 and len(versions) < 250:
            commands.append(f"F {version}")
            versions.append(versions[version].copy())
            answers.append(len(versions) - 1)
        elif action < 74:
            index = rng.choice([0, SIZE - 1, rng.randrange(SIZE)])
            commands.append(f"G {version} {index}")
            answers.append(versions[version].get(index, 0))
        elif action < 92:
            left, right = random_range(rng)
            commands.append(f"P {version} {left} {right}")
            answers.append(range_sum(versions[version], left, right))
        else:
            commands.append(f"A {version}")
            answers.append(sum(versions[version].values()))
    write(out_dir, commands, answers)


def generate_partial(out_dir: Path, wide: bool) -> None:
    rng = random.Random(20260911 + wide)
    versions: list[dict[int, int]] = [{}]
    commands: list[str] = []
    answers: list[int] = []
    for _ in range(190):
        action = rng.randrange(100)
        if action < 55:
            index = rng.choice([0, SIZE - 1, rng.randrange(SIZE)])
            value = rng.randrange(-10**9, 10**9 + 1)
            state = versions[-1].copy()
            add = wide and rng.randrange(2) == 0
            state[index] = state.get(index, 0) + value if add else value
            if state[index] == 0:
                state.pop(index)
            versions.append(state)
            commands.append(f"{'D' if add else 'S'} {index} {value}")
            answers.append(len(versions) - 1)
        else:
            version = rng.randrange(len(versions))
            if action < 72:
                index = rng.choice([0, SIZE - 1, rng.randrange(SIZE)])
                commands.append(f"G {version} {index}")
                answers.append(versions[version].get(index, 0))
            elif action < 92:
                left, right = random_range(rng)
                commands.append(f"P {version} {left} {right}")
                answers.append(range_sum(versions[version], left, right))
            else:
                commands.append(f"A {version}")
                answers.append(sum(versions[version].values()))
    write(out_dir, commands, answers)


def generate_rollback(out_dir: Path, wide: bool) -> None:
    rng = random.Random(20260921 + wide)
    states: list[dict[int, int]] = [{}]
    commands: list[str] = []
    answers: list[int] = []
    for _ in range(220):
        action = rng.randrange(100)
        if action < 48:
            index = rng.choice([0, SIZE - 1, rng.randrange(SIZE)])
            value = rng.randrange(-10**9, 10**9 + 1)
            state = states[-1].copy()
            add = wide and rng.randrange(2) == 0
            state[index] = state.get(index, 0) + value if add else value
            if state[index] == 0:
                state.pop(index)
            states.append(state)
            commands.append(f"{'D' if add else 'S'} {index} {value}")
        elif action < 57 and len(states) > 1:
            commands.append("U")
            states.pop()
        elif action < 67:
            target = rng.randrange(len(states))
            commands.append(f"R {target}")
            states = states[: target + 1]
        elif action < 75:
            commands.append("T")
            answers.append(len(states) - 1)
        elif action < 84:
            index = rng.choice([0, SIZE - 1, rng.randrange(SIZE)])
            commands.append(f"G {index}")
            answers.append(states[-1].get(index, 0))
        elif action < 95:
            left, right = random_range(rng)
            commands.append(f"P {left} {right}")
            answers.append(range_sum(states[-1], left, right))
        else:
            commands.append("A")
            answers.append(sum(states[-1].values()))
    write(out_dir, commands, answers)
