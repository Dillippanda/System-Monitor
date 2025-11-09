*.o
sysmon
.vscode/
```

3. Add all files to Git:
```bash
git add Makefile
git add src/*.cpp
git add include/*.h
```

4. Commit the files:
```bash
git commit -m "Initial commit"
```

Alternatively, you can add all files at once:
```bash
git add .
git commit -m "Initial commit"
```

Make sure you're in the project root directory (c:\Users\VICTUS\OneDrive\Desktop\SystemMonitor) when running these commands.# filepath: c:\Users\VICTUS\OneDrive\Desktop\SystemMonitor\.gitignore
*.o
sysmon
.vscode/
```

3. Add all files to Git:
```bash
git add Makefile
git add src/*.cpp
git add include/*.h
```

4. Commit the files:
```bash
git commit -m "Initial commit"
```

Alternatively, you can add all files at once:
```bash
git add .
git commit -m "Initial commit"
```

Make sure you're in the project root directory (c:\Users\VICTUS\OneDrive\Desktop\SystemMonitor) when running these commands.CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Iinclude
LDFLAGS = -lncurses
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = sysmon

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

