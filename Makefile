build:
	gcc -W -o ./bin/fluids ./src/*.c -I"./include" -L"./lib" -lglfw3 -lopengl32 -lgdi32

run:
	@echo compiling...
	@gcc -W -o ./bin/fluids ./src/*.c -I"./include" -L"./lib" -lglfw3 -lopengl32 -lgdi32
	@echo running...
	./bin/fluids
