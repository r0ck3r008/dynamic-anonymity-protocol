export I_PATH=$(PWD)
export COMPILER=gcc
export CLIENT_BIN=$(I_PATH)/bin/client.elf
export PEER_BIN=$(I_PATH)/bin/peer.elf
export DBINTERFACE_BIN=$(I_PATH)/bin/dbinterface.elf
export SERVER_BIN=$(I_PATH)/bin/server.elf

all: $(CLIENT_BIN) $(PEER_BIN) $(DBINTERFACE_BIN) $(SERVER_BIN)

$(CLIENT_BIN): $(I_PATH)/client.d/Makefile
	make -C $(I_PATH)/client.d

$(PEER_BIN): $(I_PATH)/peer.d/Makefile
	make -C $(I_PATH)/peer.d 

$(DBINTERFACE_BIN): $(I_PATH)/dbinterface.d/Makefile
	make -C $(I_PATH)/dbinterface.d

$(SERVER_BIN): $(I_PATH)/generic-server.d/Makefile
	make -C $(I_PATH)/generic-server.d

clean:
	rm $(I_PATH)/bin/*.elf
