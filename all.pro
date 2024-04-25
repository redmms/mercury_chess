TEMPLATE = subdirs
#CONFIG += ordered

modules.file = modules.pro
modules.target = obj_bitremedy
SUBDIRS += modules

app.file = app.pro
app.depends = modules
SUBDIRS += app

