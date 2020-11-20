FLAGS = -w -g -fpermissive -std=c++17 -pthread -ldl
CC = g++
TARGET = async_test

SOURCES = async_task.cc main.cc
OBJECTS = $(patsubst %.cc, %.o, $(SOURCES))

.PHONY: clean all

all: $(OBJECTS)
	$(CC) $(FLAGS) -o $(TARGET) $^

$(OBJECTS) : %.o : %.cc
	$(CC) $(FLAGS) -c -o $@ $^

clean:
	rm -rf $(TARGET)  ./*.o
	
