import sys, os
import compizconfig

configfile=sys.argv[-1]

context=compizconfig.Context()
context.Import(os.path.abspath(configfile))

grid_settings = sorted([setting for setting in context.Plugins['grid'].Screen])
grid_setting_values = [context.Plugins['grid'].Screen[setting].Value for setting in grid_settings]
for s, v in zip(grid_settings, grid_setting_values):
    print("{}: {}".format(s, v))
