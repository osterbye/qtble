TEMPLATE = subdirs

SUBDIRS = \
    example \
    qtble

example.file = example/example_simple/example_simple.pro
qtble.file = qtble/qtble.pro

example.depends = qtble
