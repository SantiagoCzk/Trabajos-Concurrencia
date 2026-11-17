CC = mpicc
CFLAGS = -Wall -Wextra -std=c99
TARGET = smoothing_descentralizado
SRC = smoothing_descentralizado.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) resultado.txt

.PHONY: all clean