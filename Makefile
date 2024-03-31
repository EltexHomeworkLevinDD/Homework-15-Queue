CFLAGS := -Wall -Wextra -g
MAKE_CD_OPTIONS := --no-print-directory

clean:
	$(MAKE) $(MAKE_CD_OPTIONS) -C $(SRC_DIR)/$(MESSAGING_DIR) clean
	$(MAKE) $(MAKE_CD_OPTIONS) -C $(SRC_DIR)/$(GRAPHICS_DIR) clean
	$(MAKE) $(MAKE_CD_OPTIONS) -C $(SRC_DIR)/$(THREADS_DIR) clean
	rm -f $(BIN_DIR)/$(TARGET)

.PHONY: all client server clean_client clean_server

all: client server

client:
	$(MAKE) $(MAKE_CD_OPTIONS) -C client all

server:
	$(MAKE) $(MAKE_CD_OPTIONS) -C server all

clean_client:
	$(MAKE) $(MAKE_CD_OPTIONS) -C client clean

clean_server:
	$(MAKE) $(MAKE_CD_OPTIONS) -C server clean

clean: clean_server clean_client