CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = shell

all: $(TARGET)

$(TARGET): src/shell.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) src/shell.cpp

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)