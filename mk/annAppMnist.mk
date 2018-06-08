SOURCES += \
	./src/ann/mlp.cpp \
	./src/proj_mnist/main.cpp

INCLUDES += \
	./src/ann \
	./include \
	./src/proj_mnist/mnist/include

CXXFLAGS = -std=c++14