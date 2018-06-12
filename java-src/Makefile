JAVAC := javac

DIR := $(shell pwd)
SRC := $(wildcard com/imkiva/kivm/*.java)
OBJ := $(SRC:.java=.class)

all: $(OBJ)
	@echo " Done"

%.class: %.java
	@echo "[Compiling] $<"
	@$(JAVAC) -d ../java-out -encoding utf-8 $<
