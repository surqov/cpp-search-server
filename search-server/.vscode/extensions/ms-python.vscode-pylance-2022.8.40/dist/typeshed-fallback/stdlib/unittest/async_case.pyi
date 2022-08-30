import sys
from collections.abc import Awaitable, Callable
from typing import TypeVar
from typing_extensions import ParamSpec

from .case import TestCase

if sys.version_info >= (3, 11):
    from contextlib import AbstractAsyncContextManager

_T = TypeVar("_T")
_P = ParamSpec("_P")

class IsolatedAsyncioTestCase(TestCase):
    async def asyncSetUp(self) -> None: ...
    async def asyncTearDown(self) -> None: ...
    def addAsyncCleanup(self, __func: Callable[_P, Awaitable[object]], *args: _P.args, **kwargs: _P.kwargs) -> None: ...
    if sys.version_info >= (3, 11):
        async def enterAsyncContext(self, cm: AbstractAsyncContextManager[_T]) -> _T: ...
