RLS?=0
ifeq ($(RLS),1)
	BUILD_TYPE:=release
	CFLAGS+=-Wall -O3
	LDFLAGS+=-static -lm
else
	BUILD_TYPE:=debug
	CFLAGS+=-Wall -g3 -DDEBUG -DDEBUG_STRESS_GC
	LDFLAGS+=-lm
endif


CC:=gcc
BUILD_DIR:=build
ANDROID_API_BUILD_DIR:=$(BUILD_DIR)/android
TARGET:=pankti
PANKTI_VERSION:=$(shell cat ./VERSION)

# Sample to Run
STORUN=sample/0.pank 

EXT_BAURINUM_SDIR:=cpank/ext/baurinum
EXT_BAURINUM_BDIR:=$(BUILD_DIR)/$(BUILD_TYPE)
EXT_BAURINUM_SRC:=$(shell find $(EXT_BAURINUM_SDIR) -maxdepth 1 -name "*.c")
EXT_BAURINUM_OBJS:=$(EXT_BAURINUM_SRC:%=$(EXT_BAURINUM_BDIR)/%.o)

STDLIB_SDIR=cpank/stdlib 
STDLIB_BDIR=$(BUILD_DIR)/$(BUILD_TYPE)
STDLIB_SRC:=$(shell find $(STDLIB_SDIR) -maxdepth 1 -name "*.c")
STDLIB_OBJS:=$(STDLIB_SRC:%=$(STDLIB_BDIR)/%.o)

CORE_SDIR=cpank/
CORE_BDIR=$(BUILD_DIR)/$(BUILD_TYPE)
CORE_SRC:=$(shell find $(CORE_SDIR) -maxdepth 1 -name "*.c")
CORE_SRC_NOMAIN:=$(shell find $(CORE_SDIR) -maxdepth 1 -name "*.c" ! -iname "main.c")
CORE_OBJS:=$(CORE_SRC:%=$(CORE_BDIR)/%.o)

TEST_SRC:=$(CORE_SRC_NOMAIN)
TEST_SRC+=testmain.c
TEST_OBJS:=$(TEST_SRC:%=$(CORE_BDIR)/%.o)

WEBMAIN_SRC:=web/pankti.web.c

OBJS:=$(EXT_BAURINUM_OBJS)
OBJS+=$(STDLIB_OBJS) 
OBJS+=$(CORE_OBJS)


PANKTI_SRC:=$(shell find $(CORE_SDIR) -maxdepth 1 -name "*.c")
PANKTI_SRC+=$(shell find $(STDLIB_SDIR) -maxdepth 1 -name "*.c")
PANKTI_INCLUDE_DIR:=$(CORE_SDIR)/include

CORE_MAIN:=$(CORE_SDIR)/main.c 
TEST_MAIN:=testmain.c
TEST_TARGET:=testpankti

all: run

run: $(TARGET)
	./$(TARGET) $(STORUN)
	

$(TARGET): $(EXT_BAURINUM_OBJS) $(STDLIB_OBJS) $(CORE_OBJS)
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(TEST_TARGET): $(EXT_BAURINUM_OBJS) $(STDLIB_OBJS) $(TEST_OBJS)
	@$(CC) $(EXT_BAURINUM_OBJS) $(STDLIB_OBJS) $(TEST_OBJS) -o $@ $(LDFLAGS)

build_uo: CFLAGS=-std=c11 -g3 -Wall -pedantic -DMODE_BENGALI
build_uo: LDFLAGS=-lm
build_uo: tommath stdlib core
build_uo: $(TARGET)

build: CFLAGS=-std=c11 -O3 -Wall -DMODE_BENGALI 
build: LDFLAGS=-static -lm
build: tommath stdlib core
build: $(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

andapi:
	mkdir -p $(ANDROID_API_BUILD_DIR)
	@echo "Building Android API"
	@echo "v${PANKTI_VERSION}"
	CGO_ENABLED=1 ANDROID_NDK_HOME=$(HOME)/Android/Sdk/ndk/25.1.8937393/ gomobile bind -v -o $(ANDROID_API_BUILD_DIR)/panktijapi-$(PANKTI_VERSION).aar -target=android -javapkg=in.palashbauri.panktijapi -androidapi 19 ./gopkg
	cp gopkg/gopkg.pom $(ANDROID_API_BUILD_DIR)//panktijapi-$(PANKTI_VERSION).pom


check:
	cppcheck --force --inline-suppr -I $(PANKTI_INCLUDE_DIR) --enable=all $(CORE_MAIN) $(PANKTI_SRC)

debug: build_uo
	gdb --args $(TARGET) $(STORUN)

memcheck: build_uo
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET) $(STORUN)

fmt:
	clang-format -i -style=file cpank/*.c cpank/include/*.h cpank/include/helper/*.h cpank/stdlib/*.c web/*.c

wasm:
	emcc $(EXT_BAURINUM_SRC) $(STDLIB_SRC) $(CORE_SRC_NOMAIN) $(WEBMAIN_SRC) -o web/pweb.js -O2 -DMODE_BENGALI -DNO_DOUBLE_BIGNUM -s WASM=1 -s EXPORTED_FUNCTIONS='["_comp_run","_main"]' -s EXPORTED_RUNTIME_METHODS='["cwrap"]' $(LDFLAGS)

core: $(CORE_OBJS)

stdlib: $(STDLIB_OBJS)

tommath: $(EXT_BAURINUM_OBJS)

$(CORE_BDIR)/%.c.o: %.c 
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


$(STDLIB_BDIR)/%.c.o: %.c 
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


$(EXT_BAURINUM_BDIR)/%.c.o: %.c 
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f ./$(TARGET)
	rm -f ./$(TEST_TARGET)
	rm -rf ./panktiw
	rm -f ./massif.out.*
	rm -rf ./perf.data
	rm -f ./perf.data.*
	rm -f ./cpank.perf 
	rm -f ./cpank.gmon.*
	rm -f ./vgcore.*
	rm -rf ./.cache/
	rm -rf *.o
	rm -rf *.out
	rm -rf dist/
	rm -rf *.callgraph.dot
	rm -rf web/pweb.js
	rm -rf web/pweb.wasm

cleanall: clean
	rm -rf ./build
