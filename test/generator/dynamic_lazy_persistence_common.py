# competitive-verifier: DISPLAY hidden
from __future__ import annotations

import random
from pathlib import Path

SIZE = 1_000_000_000_039
Segment = tuple[int, int, int]


def normalize(segments: list[Segment]) -> list[Segment]:
    result: list[Segment] = []
    for left, right, value in segments:
        if left == right:
            continue
        if result and result[-1][1] == left and result[-1][2] == value:
            result[-1] = (result[-1][0], right, value)
        else:
            result.append((left, right, value))
    return result


def range_add(segments: list[Segment], query_left: int, query_right: int, value: int) -> list[Segment]:
    result: list[Segment] = []
    for left, right, current in segments:
        if right <= query_left or query_right <= left:
            result.append((left, right, current))
            continue
        if left < query_left:
            result.append((left, query_left, current))
        middle_left = max(left, query_left)
        middle_right = min(right, query_right)
        result.append((middle_left, middle_right, current + value))
        if query_right < right:
            result.append((query_right, right, current))
    return normalize(result)


def point_set(segments: list[Segment], index: int, value: int) -> list[Segment]:
    current = point_get(segments, index)
    return range_add(segments, index, index + 1, value - current)


def point_get(segments: list[Segment], index: int) -> int:
    return next(value for left, right, value in segments if left <= index < right)


def range_sum(segments: list[Segment], query_left: int, query_right: int) -> int:
    return sum((min(right, query_right) - max(left, query_left)) * value for left, right, value in segments if left < query_right and query_left < right)


def write(out_dir: Path, commands: list[str], answers: list[int]) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "case_00.in").write_text(f"{len(commands)} {SIZE}\n" + "\n".join(commands) + "\n", encoding="utf-8")
    (out_dir / "case_00.out").write_text("\n".join(map(str, answers)) + "\n", encoding="utf-8")


def random_range(rng: random.Random) -> tuple[int, int]:
    if rng.randrange(5) == 0:
        return 0, SIZE
    left = rng.randrange(SIZE + 1)
    right = rng.randrange(left, SIZE + 1)
    return left, right


def generate_persistent(out_dir: Path) -> None:
    rng = random.Random(20260931)
    versions: list[list[Segment]] = [[(0, SIZE, 0)]]
    commands: list[str] = []
    answers: list[int] = []

    def update(command: str, state: list[Segment]) -> None:
        commands.append(command); versions.append(state); answers.append(len(versions) - 1)

    update(f"R 0 0 {SIZE} 1", range_add(versions[0], 0, SIZE, 1))
    update(f"S 1 {SIZE - 40} 5", point_set(versions[1], SIZE - 40, 5))
    update("R 0 10 20 3", range_add(versions[0], 10, 20, 3))
    commands.extend(["A 0", "A 1", "A 2", f"G 1 {SIZE - 40}", f"G 2 {SIZE - 40}", "P 3 10 20"])
    answers.extend([0, SIZE, SIZE + 4, 1, 5, 30])

    for _ in range(155):
        action = rng.randrange(100)
        version = rng.randrange(len(versions))
        if action < 22 and len(versions) < 245:
            index = rng.choice([0, SIZE - 1, rng.randrange(SIZE)])
            value = rng.randrange(-20, 21)
            update(f"S {version} {index} {value}", point_set(versions[version], index, value))
        elif action < 32 and len(versions) < 245:
            index = rng.choice([0, SIZE - 1, rng.randrange(SIZE)])
            value = rng.randrange(-5, 6)
            update(f"D {version} {index} {value}", range_add(versions[version], index, index + 1, value))
        elif action < 58 and len(versions) < 245:
            left, right = random_range(rng); value = rng.randrange(-5, 6)
            update(f"R {version} {left} {right} {value}", range_add(versions[version], left, right, value))
        elif action < 64 and len(versions) < 245:
            commands.append(f"F {version}"); versions.append(versions[version].copy()); answers.append(len(versions) - 1)
        elif action < 76:
            index = rng.choice([0, SIZE - 1, rng.randrange(SIZE)])
            commands.append(f"G {version} {index}"); answers.append(point_get(versions[version], index))
        elif action < 94:
            left, right = random_range(rng)
            commands.append(f"P {version} {left} {right}"); answers.append(range_sum(versions[version], left, right))
        else:
            commands.append(f"A {version}"); answers.append(range_sum(versions[version], 0, SIZE))
    write(out_dir, commands, answers)


def generate_partial(out_dir: Path) -> None:
    rng = random.Random(20261001)
    versions: list[list[Segment]] = [[(0, SIZE, 0)]]
    commands: list[str] = []
    answers: list[int] = []
    for _ in range(170):
        action = rng.randrange(100)
        if action < 60:
            state = versions[-1]
            if action < 22:
                index = rng.choice([0, SIZE - 1, rng.randrange(SIZE)]); value = rng.randrange(-20, 21)
                commands.append(f"S {index} {value}"); versions.append(point_set(state, index, value))
            elif action < 34:
                index = rng.choice([0, SIZE - 1, rng.randrange(SIZE)]); value = rng.randrange(-5, 6)
                commands.append(f"D {index} {value}"); versions.append(range_add(state, index, index + 1, value))
            else:
                left, right = random_range(rng); value = rng.randrange(-5, 6)
                commands.append(f"R {left} {right} {value}"); versions.append(range_add(state, left, right, value))
            answers.append(len(versions) - 1)
        else:
            version = rng.randrange(len(versions))
            if action < 75:
                index = rng.choice([0, SIZE - 1, rng.randrange(SIZE)])
                commands.append(f"G {version} {index}"); answers.append(point_get(versions[version], index))
            elif action < 94:
                left, right = random_range(rng)
                commands.append(f"P {version} {left} {right}"); answers.append(range_sum(versions[version], left, right))
            else:
                commands.append(f"A {version}"); answers.append(range_sum(versions[version], 0, SIZE))
    write(out_dir, commands, answers)


def generate_rollback(out_dir: Path) -> None:
    rng = random.Random(20261011)
    states: list[list[Segment]] = [[(0, SIZE, 0)]]
    commands: list[str] = []
    answers: list[int] = []
    for _ in range(190):
        action = rng.randrange(100)
        if action < 54:
            state = states[-1]
            if action < 20:
                index = rng.choice([0, SIZE - 1, rng.randrange(SIZE)]); value = rng.randrange(-20, 21)
                commands.append(f"S {index} {value}"); states.append(point_set(state, index, value))
            elif action < 31:
                index = rng.choice([0, SIZE - 1, rng.randrange(SIZE)]); value = rng.randrange(-5, 6)
                commands.append(f"D {index} {value}"); states.append(range_add(state, index, index + 1, value))
            else:
                left, right = random_range(rng); value = rng.randrange(-5, 6)
                commands.append(f"R {left} {right} {value}"); states.append(range_add(state, left, right, value))
        elif action < 61 and len(states) > 1:
            commands.append("U"); states.pop()
        elif action < 70:
            target = rng.randrange(len(states)); commands.append(f"B {target}"); states = states[: target + 1]
        elif action < 78:
            commands.append("T"); answers.append(len(states) - 1)
        elif action < 85:
            index = rng.choice([0, SIZE - 1, rng.randrange(SIZE)])
            commands.append(f"G {index}"); answers.append(point_get(states[-1], index))
        elif action < 96:
            left, right = random_range(rng)
            commands.append(f"P {left} {right}"); answers.append(range_sum(states[-1], left, right))
        else:
            commands.append("A"); answers.append(range_sum(states[-1], 0, SIZE))
    write(out_dir, commands, answers)
