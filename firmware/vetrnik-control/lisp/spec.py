#!/usr/bin/env python3
import yaml
import logging
import jinja2
from dataclasses import dataclass
from enum import Enum
from typing import Optional


_LOGGER = logging.getLogger(__name__)


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

    def get_code(self) -> str:
        """Get LISP code for the test."""
        if _code is not None:
            return _code
        else:
            pass
            # TODO render template

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
    return Spec(
        functions={
            (fn := Function.from_dict(f)).name: fn
            for f in d["functions"]
        },
        categories=d.get("categories", {})
    )


def parse_file(filename: str) -> Spec:
    with open(filename, "r") as f:
        d = yaml.safe_load(f)
    _LOGGER.debug("parsed yaml: %r", d)
    return parse_dict(d)


def render_markdown(spec: Spec) -> str:
    env = jinja2.Environment(autoescape=jinja2.select_autoescape(),
                             loader=jinja2.FileSystemLoader("."))
    template = env.get_template("spec.md.jinja")
    return template.render(functions=spec.functions,
                           categories=spec.categories)


def main():
    spec = parse_file("spec.yaml")
    _LOGGER.debug("parsed spec: %r", spec)
    md = render_markdown(spec)
    with open("spec.md", "w") as f:
        f.write(md)


if __name__ == "__main__":
    main()
