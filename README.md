# C fluid sim

a fluid simulator written in c

> :warning: this sim is unstable, if it blows up just relaunch, might fix later

## how to use:

```console
$ cd ./c-fluid
$ make run
```

OR

```console
$ cd ./c-fluid
$ gcc -W -o ./bin/fluids ./src/*.c -I"./include" -L"./lib" -lglfw3 -lopengl32 -lgdi32
$ ./bin/fluids
```

## material

[Stable Fluids](https://www.researchgate.net/publication/2486965_Stable_Fluids)\
[Real-Time Fluid Dynamics for Games](https://www.researchgate.net/publication/2560062_Real-Time_Fluid_Dynamics_for_Games)\
[Fluid Simulation For Dummies](https://mikeash.com/pyblog/fluid-simulation-for-dummies.html)\
[OpenGL](https://learnopengl.com/)
