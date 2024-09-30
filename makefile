# Define compiler and flags
CXX = g++
CXXFLAGS = -I"./include" -I"./external/imgui" -I"./external/imgui-sfml" -L"./lib" -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lglu32

# Source files
SRC = main.cpp \
      external/imgui/imgui.cpp \
      external/imgui/imgui_draw.cpp \
      external/imgui/imgui_widgets.cpp \
      external/imgui/imgui_tables.cpp \
      external/imgui/imgui_demo.cpp \
      external/imgui-sfml/imgui-SFML.cpp

# Output executable
TARGET = temple_renderer

all:
	$(CXX) -o $(TARGET) $(SRC) $(CXXFLAGS)

clean:
	rm -f $(TARGET)

# one line compilation if you don't have make
# g++ -o temple_renderer main.cpp external/imgui/imgui.cpp external/imgui/imgui_draw.cpp external/imgui/imgui_widgets.cpp external/imgui/imgui_tables.cpp external/imgui/imgui_demo.cpp external/imgui-sfml/imgui-SFML.cpp -I"./include" -I"./external/imgui" -I"./external/imgui-sfml" -L"./lib" -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lglu32
