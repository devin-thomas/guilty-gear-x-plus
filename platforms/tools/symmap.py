"""Read a port symbol map (platforms/common/core/symbols/<kr|jp>.txt, from export_symbols.py)."""
from pathlib import Path


class SymbolMap:
    def __init__(self, path):
        self.globals, self.statics, self.repeated = {}, {}, set()
        # the game's statics by TU (statics.py): (TU, Core name) -> PS2 address
        self.tu_statics = {}
        statics = Path(path).with_name(f'statics_{Path(path).stem}.txt')
        if statics.exists():
            for line in statics.read_text().splitlines():
                if line and not line.startswith('#'):
                    tu, name, addr = line.split()
                    self.tu_statics[(tu, name)] = int(addr, 16)
        for line in Path(path).read_text().splitlines():
            if not line or line.startswith('#'):
                continue
            name, addr, kind, binding = line.split()
            if binding == 'R':
                self.repeated.add(name)
            else:
                (self.globals if binding == 'G' else self.statics)[name] = (int(addr, 16), kind == 'F')

    def address(self, name, is_function, local):
        """PS2 address for a symbol the Core defines or references, or None.

        A Core global is looked up as a PS2 global first (the game C often
        declares a PS2 file static extern, by a solved name). A Core file
        static uses the PS2 file static of that name if there is exactly one.
        wasm_place.py asks static() first for a static of one TU."""
        if local:
            order = [self.statics] if name in self.repeated else [self.statics, self.globals]
        else:
            order = [self.globals, self.statics]
        for table in order:
            hit = table.get(name)
            if hit and hit[1] == is_function:
                return hit[0]
        return None

    def static(self, tu, name):
        """PS2 address of a static the Core's object for TU defines (a file static or `<function>.<var>`),
        from statics_<release>.txt, or None."""
        return self.tu_statics.get((tu, name))
