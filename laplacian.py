import sympy as sym

x,y = sym.symbols("x y")
alpha,beta,gamma = sym.symbols(r"\alpha \beta \gamma")
h = sym.exp(-alpha*(x**2 + y**2))

lap = (-sym.diff(h, x, x) - sym.diff(h, y, y)).expand()
lprint("", h)
lprint("", lap)
lprint("", lap.subs(alpha, 2).simplify())
lprint("", sym.diff(h,x,x).subs(alpha, 2).simplify())
# lprint("", sym.ccode(lap))

h = x**2 - y**2
hxx = sym.diff(h, x,x)
hyy = sym.diff(h, y,y)
lprint("", hxx)
lprint("", hyy)
lprint("", hxx+hyy)

ldone()

