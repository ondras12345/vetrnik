#!/usr/bin/env python3
"""diff the contents of two linearization pickles"""
from linearize_result import Result  # noqa: F401
import pickle
import argparse

def diff_lists(l1, l2):
    if len(l1) != len(l2):
        print(f"list lengths differ: {len(l1)} != {len(l2)}")
    for i, (e1, e2) in enumerate(zip(l1, l2)):
        if e1 != e2:
            print(f"elements {i} differ:")
            print(f"e1={repr(e1)}")
            print(f"e2={repr(e2)}")


def diff_dicts(d1, d2):
    if d1.keys() != d2.keys():
        print(f"keys are different:\nd1={d1}\nd2={d2}")
    else:
        for key, value1 in d1.items():
            value2 = d2[key]
            if value1 != value2:
                if isinstance(value1, list):
                    print(f"values for key {repr(key)} are different lists:")
                    diff_lists(value1, value2)
                else:
                    print(f"values for key {repr(key)} are different:\nvalue1={value1}\nvalue2={value2}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("pickle1", type=argparse.FileType("rb"))
    parser.add_argument("pickle2", type=argparse.FileType("rb"))
    args = parser.parse_args()

    with args.pickle1 as f:
        p1 = pickle.load(f)

    with args.pickle2 as f:
        p2 = pickle.load(f)

    if isinstance(p1, dict) and isinstance(p2, dict):
        diff_dicts(p1, p2)
    else:
        diff_lists(p1, p2)

    print("done")
