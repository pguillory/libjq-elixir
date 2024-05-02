.PHONY: nifs clean publish

ERL_PATH = $(shell elixir -e 'IO.puts [:code.root_dir, "/erts-", :erlang.system_info :version]')
CFLAGS := -fPIC -I $(ERL_PATH)/include -O3

UNAME_SYS := $(shell uname -s)
ifeq ($(UNAME_SYS), Darwin)
	CFLAGS += -bundle -bundle_loader $(ERL_PATH)/bin/beam.smp -I /opt/homebrew/include -L/opt/homebrew/lib
else ifeq ($(UNAME_SYS), FreeBSD)
	CFLAGS += -shared
else ifeq ($(UNAME_SYS), Linux)
	CFLAGS += -shared
endif

nifs: priv/jq.so

priv:
	mkdir -p priv

priv/jq.so: priv src/jq.c
	gcc -o priv/jq.so src/jq.c $(CFLAGS) -ljq

clean:
	rm -f priv/jq.so

publish: clean
	mix hex.publish --yes
