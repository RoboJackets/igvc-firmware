IGNORE_FOLDERS:=cmake-build-debug build mbed_config.h
SUBMODULES:=$(shell grep 'path = ' .gitmodules | cut -d= -f2-)
IGNORE:= $(IGNORE_FOLDERS) $(SUBMODULES)
SUFFIXED_SUBMODULES:=$(addsuffix %%,$(IGNORE))
PATHED_SUBMODULES:=$(addprefix -path ./,$(SUFFIXED_SUBMODULES))
SUBSTED_SUBMODULES:=$(subst %%, -prune -o ,$(PATHED_SUBMODULES))
FILES:=$(shell find . $(SUBSTED_SUBMODULES) \( -name '*.h' -or -name '*.hpp' -or -name '*.cpp' \) -print)

format:
	clang-format -i $(FILES)