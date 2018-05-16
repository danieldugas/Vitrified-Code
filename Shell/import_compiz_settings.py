import sys, os
import compizconfig

configfile=sys.argv[-1]

context=compizconfig.Context()
context.Import(os.path.abspath(configfile))
