CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall
INCLUDES = -I./tracer -I./reader

SRC = simulate.cpp \
      tracer/photon.cpp \
      tracer/scene.cpp \
      reader/phase_function_reader.cpp \
      reader/wavelengths_model.cpp

OBJDIR = obj
OBJ = $(patsubst %.cpp, $(OBJDIR)/%.o, $(SRC))

TARGET = simulate 

BANNER_TARGET = interface/print_banner
BANNER_SRC = interface/print_banner.cpp

all: $(TARGET) $(BANNER_TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BANNER_TARGET): $(BANNER_SRC) interface/banner.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(BANNER_SRC)

clean:
	rm -rf $(OBJDIR) $(TARGET) $(BANNER_TARGET)
