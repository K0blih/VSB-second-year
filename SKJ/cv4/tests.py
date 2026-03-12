import tempfile

import pytest
from tasks import Vector, UpperCaseDecorator, Observable, GameOfLife


def test_vector_init():
    vec = Vector(1.2, 3.4, 5.6)
    assert vec.x == 1.2
    assert vec.y == 3.4
    assert vec.z == 5.6

    vec = Vector()
    assert vec.x == 0
    assert vec.y == 0
    assert vec.z == 0

    vec = Vector(z=3)
    assert vec.x == 0
    assert vec.y == 0
    assert vec.z == 3


def test_vector_add():
    a = Vector(1, 2, 3)
    b = Vector(3, 4, 5)
    c = a + b

    assert a.x == 1
    assert a.y == 2
    assert a.z == 3
    assert b.x == 3
    assert b.y == 4
    assert b.z == 5
    assert c.x == 4
    assert c.y == 6
    assert c.z == 8

    with pytest.raises(ValueError):
        Vector(1, 2, 3) + 5


def test_vector_sub():
    a = Vector(1, 2, 3)
    b = Vector(3, 4, 5)
    c = a - b

    assert a.x == 1
    assert a.y == 2
    assert a.z == 3
    assert b.x == 3
    assert b.y == 4
    assert b.z == 5
    assert c.x == -2
    assert c.y == -2
    assert c.z == -2

    with pytest.raises(ValueError):
        Vector(1, 2, 3) - 5


def test_vector_eq():
    assert Vector(0, 0, 0) == Vector(0, 0, 0)
    assert Vector(1, 2, 3) == Vector(1, 2, 3)
    assert Vector(1, 2, 3) != Vector(1, 2, 4)
    assert Vector(1, 2, 3) != 5


def test_vector_str():
    assert str(Vector(0, 0, 0)) == "(0, 0, 0)"
    assert str(Vector(1, 2, 3)) == "(1, 2, 3)"


def test_vector_indexing():
    v = Vector(1, 2, 3)
    assert v[0] == 1
    assert v[2] == 3
    v[1] = 5
    assert v[1] == 5
    assert v.y == 5

    with pytest.raises(IndexError):
        a = v[10]

    with pytest.raises(IndexError):
        v[8] = 5


def test_vector_iteration():
    v = Vector(1, 2, 3)
    assert list(v) == [1, 2, 3]
    assert list(v) == [1, 2, 3]

    it = iter(v)
    assert it is not v

    it2 = iter(v)

    assert next(it) == 1
    assert next(it2) == 1


def test_observable_complex():
    obs = Observable()

    calls = [0, 0]

    def fn1(x):
        calls[0] += x

    def fn2(x):
        calls[1] += x

    unsub1 = obs.subscribe(fn1)
    unsub2 = obs.subscribe(fn2)

    assert calls == [0, 0]

    obs.notify(5)
    assert calls == [5, 5]

    unsub1()
    obs.notify(6)
    assert calls == [5, 11]
    unsub2()

    obs.notify(3)
    assert calls == [5, 11]

    args = [None, None]

    def fn3(*p, **kw):
        args[0] = p
        args[1] = kw

    obs.subscribe(fn3)
    obs.notify(1, 2, 3, a=5, b=6)
    assert args == [(1, 2, 3), {"a": 5, "b": 6}]


def test_upper_case_decorator():
    with tempfile.NamedTemporaryFile(mode="w+") as f:
        decorator = UpperCaseDecorator(f)
        decorator.write("Hello World\n")
        decorator.writelines(["Prefer\n", "compOSItioN OVeR\n", "INHERITance\n"])
        f.seek(0)
        assert """ELLO ORLD
REFER
COMPTIO E
ANCE
""" == f.read()


def test_game_of_life_count():
    g = GameOfLife((
        ('.', '.', '.'),
        ('.', '.', '.'),
        ('.', '.', '.')
    ))
    assert g.alive() == 0
    assert g.dead() == 9

    g = GameOfLife((
        ('x', '.', '.'),
        ('.', 'x', '.'),
        ('.', 'x', 'x')
    ))
    assert g.alive() == 4
    assert g.dead() == 5


def test_game_of_life_move():
    assert GameOfLife((
        ('.', '.', '.'),
        ('.', 'x', '.'),
        ('.', 'x', '.'),
        ('.', 'x', '.'),
        ('.', '.', '.')
    )).move().board == (
        ('.', '.', '.'),
        ('.', '.', '.'),
        ('x', 'x', 'x'),
        ('.', '.', '.'),
        ('.', '.', '.')
    )

    assert GameOfLife((
        ('.', '.', '.', '.'),
        ('.', 'x', 'x', '.'),
        ('.', 'x', 'x', '.'),
        ('.', '.', '.', '.'),
    )).move().board == (
        ('.', '.', '.', '.'),
        ('.', 'x', 'x', '.'),
        ('.', 'x', 'x', '.'),
        ('.', '.', '.', '.'),
    )

    assert GameOfLife((
        ('x', '.', '.'),
        ('.', 'x', 'x'),
        ('x', 'x', '.'),
    )).move().board == (
        ('.', 'x', '.'),
        ('.', '.', 'x'),
        ('x', 'x', 'x'),
    )


def test_game_of_life_move_immutable():
    board = (
        ('.', '.', '.'),
        ('.', 'x', '.'),
        ('.', 'x', '.'),
        ('.', 'x', '.'),
        ('.', '.', '.')
    )

    g = GameOfLife(board)
    n = g.move()
    assert id(n.board) != id(board)
    assert board == (
        ('.', '.', '.'),
        ('.', 'x', '.'),
        ('.', 'x', '.'),
        ('.', 'x', '.'),
        ('.', '.', '.')
    )
    assert n.board == (
        ('.', '.', '.'),
        ('.', '.', '.'),
        ('x', 'x', 'x'),
        ('.', '.', '.'),
        ('.', '.', '.')
    )


def test_game_of_life_print():
    g = GameOfLife((
        ('x', '.', '.'),
        ('.', 'x', '.'),
        ('.', 'x', 'x')
    ))
    assert str(g) == "x..\n.x.\n.xx\n"
