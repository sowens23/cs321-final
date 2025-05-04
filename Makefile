CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = shell

all: $(TARGET)

$(TARGET): shell.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) shell.cpp

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)