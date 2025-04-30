#!/usr/bin/env python3
import argparse
import yaml
import logging
import jinja2
import pathlib
import sexpdata
import sys
from dataclasses import dataclass
from enum import Enum
from typing import Optional


TESTS_DIR = pathlib.Path("tests")
_LOGGER = logging.getLogger(__name__)

jinja_env = jinja2.Environment(autoescape=jinja2.select_autoescape(),
                               loader=jinja2.FileSystemLoader("."))


class Type(Enum):
    NUMBER = "number"  # fe_Number
    STRING = "string"
    BOOL = "bool"  # either a true value or nil
    NIL = "nil"
    ANY = "any"  # multiple different types are acceptable


@dataclass
class Argument:
    name: str
    type: Optional[Type]
    values: Optional[list[str]] = None
    description: str = ""

    @classmethod
    def from_dict(cls, d):
        return cls(
            name=d["name"],
            type=Type(d.get("type", Type.ANY)),
            values=d.get("values"),
            description=d.get("description", "")
        )


@dataclass
class Test:
    _code: Optional[str] = None
    _template: Optional[str] = None

    def get_code(self, fn: "Function") -> str:
        """Get LISP code for the test."""
        if self._code is not None:
            code = self._code
        else:
            code = jinja_env.overlay(autoescape=False).from_string(self._template).render(this=fn)

        sexpdata.loads(code)  # check for unbalanced parens
        return code

    @classmethod
    def from_dict(cls, d):
        if isinstance(d, str):
            return cls(_code=d)
        else:
            return cls(_template=d["template"])


@dataclass
class Function:
    name: str
    description: str
    example: str
    nargs_min: int
    nargs_max: int  # -1 means unlimited
    args: list[Argument]
    return_type: Type
    return_description: str
    category: Optional[str]
    tests: list[Test]

    @staticmethod
    def validate_nargs(count: int, nargs_min: int, nargs_max: int) -> bool:
        if count > nargs_max and not nargs_max < 0:
            return False
        if count < nargs_min:
            return False
        return True

    @classmethod
    def from_dict(cls, d):
        nargs = d["nargs"]
        if isinstance(nargs, int):
            nargs_min = nargs
            nargs_max = nargs
        else:
            nargs_min = int(nargs.get("min", 0))
            nargs_max = int(nargs.get("max", -1))

        ret = d["return"]
        try:
            return_type = Type(ret)
            return_description = ""
        except ValueError:
            return_type = Type(ret["type"])
            return_description = ret.get("description", "")

        args = [Argument.from_dict(a) for a in d["args"]]
        name = d["name"]

        if not cls.validate_nargs(len(args), nargs_min, nargs_max):
            raise Exception(f"{name}: len(args) does not match nargs")

        return cls(
            name=name,
            description=d.get("description", ""),
            example=d.get("example", ""),
            category=d.get("category"),
            nargs_min=nargs_min,
            nargs_max=nargs_max,
            args=args,
            return_type=return_type,
            return_description=return_description,
            tests=[Test.from_dict(t) for t in d.get("tests", [])]
        )


@dataclass
class Spec:
    functions: dict[str, Function]
    categories: dict[str, str]  # name, description


def parse_dict(d) -> Spec:
    s = Spec(
        functions={
            (fn := Function.from_dict(f)).name: fn
            for f in d["functions"]
        },
        categories=d.get("categories", {})
    )

    # validity checks
    for _, f in s.functions.items():
        if f.category is not None and f.category not in s.categories:
            raise Exception(f"undefined category '{f.category}' used in function '{f.name}'")

    return s


def parse_file(filename: str) -> Spec:
    with open(filename, "r") as f:
        d = yaml.safe_load(f)
    _LOGGER.debug("parsed yaml: %r", d)
    return parse_dict(d)


def render_template(spec: Spec, template) -> str:
    return template.render(functions=spec.functions, categories=spec.categories)


def render_template_file(spec: Spec, filename: pathlib.Path) -> None:
    assert filename.suffix == ".jinja"
    r = render_template(spec, jinja_env.get_template(filename.as_posix()))
    out_file = filename.with_suffix("")
    out_file.write_text(r)


def render_template_str(spec: Spec, template: str) -> str:
    return render_template(spec, jinja_env.from_string(template))


def generate_tests(spec: Spec) -> None:
    TESTS_DIR.mkdir(exist_ok=True)

    for _, fn in spec.functions.items():
        if fn.tests == []:
            _LOGGER.warning(f"no tests for {fn.name}")
            continue

        try:
            test_code = jinja_env.get_template("test.lisp.jinja").render(fn=fn)
        except Exception as e:
            raise Exception(f"error rendering tests for '{fn.name}'") from e

        with open(TESTS_DIR / f"test_{fn.name}.lisp", "w") as f:
            f.write(test_code)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--render-file", "-f", type=pathlib.Path, action="append",
        help="render template from this file. The result will be written to "
             "the same location, but without the .jinja suffix. "
             "can be specified multiple times."
    )
    parser.add_argument(
        "--render-stdin", action="store_true",
        help="read a template from stdin, render it, and output the result "
             "to stdout"
    )
    parser.add_argument("--generate-tests", action="store_true")
    args = parser.parse_args()

    # either way, parse and validate the spec
    wd = pathlib.Path(__file__).resolve().parent
    spec = parse_file(wd / "spec.yaml")
    _LOGGER.debug("parsed spec: %r", spec)

    if args.generate_tests:
        generate_tests(spec)

    for f in args.render_file or []:
        render_template_file(spec, f)

    if args.render_stdin:
        template = sys.stdin.read()
        print(render_template_str(spec, template))


if __name__ == "__main__":
    main()
