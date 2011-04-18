import sys
import gdb

# Update module path.
dir = '/users/phai/share/GSTReamer_compilation/share/glib-2.0/gdb'
if not dir in sys.path:
    sys.path.insert(0, dir)

from glib import register
register (gdb.current_objfile ())
