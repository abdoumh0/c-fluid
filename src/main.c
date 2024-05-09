#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <fluid.h>
#include <screen.h>
#include <math.h>

double clamp(double x, double min, double max);

void processInput(GLFWwindow *window);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Fluids", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }

    // loading shaders

    FILE *vertex = fopen("./shaders/vertex.glsl", "rb");
    fseek(vertex, 0, SEEK_END);
    size_t vsize = ftell(vertex);
    fseek(vertex, 0, SEEK_SET);
    char *vertex_source = malloc(vsize + 1);
    fread(vertex_source, vsize, 1, vertex);
    fclose(vertex);
    vertex_source[vsize] = 0;

    FILE *fragment = fopen("./shaders/fragment.glsl", "rb");
    fseek(fragment, 0, SEEK_END);
    size_t fsize = ftell(fragment);
    fseek(fragment, 0, SEEK_SET);
    char *fragment_source = malloc(fsize + 1);
    fread(fragment_source, fsize, 1, fragment);
    fclose(fragment);
    fragment_source[fsize] = 0;

    // build and compile our shader program
    // const char *vertexShaderSource = "#version 330 core\n"

    //                                  "layout (location = 0) in vec3 aPos;\n"
    //                                  "layout (location = 1) in vec2 aTexCoord;\n"

    //                                  "out vec2 TexCoord;\n"

    //                                  "void main()\n"
    //                                  "{\n"
    //                                  "   gl_Position = vec4(aPos, 1.0);\n"
    //                                  "   TexCoord = aTexCoord.xy;\n"
    //                                  "}\0";

    // const char *fragmentShaderSource = "#version 330 core\n"

    //                                    "out vec4 FragColor;\n"
    //                                    "in vec2 TexCoord;\n"

    //                                    "uniform sampler2D texture;\n"
    //                                    "uniform vec2 u_resolution;\n"

    //                                    "void main()\n"
    //                                    "{\n"
    //                                    "    vec2 uv = gl_FragCoord.xy/u_resolution.xy;\n"
    //                                    "    float densityValue = texture(texture, TexCoord).r;\n"
    //                                    "    FragColor = vec4(densityValue*uv.x, densityValue*uv.y, densityValue, 1.0);\n"
    //                                    "}\n\0";
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // uncomment the shaders above and use them if u get a shader err
    glShaderSource(vertexShader, 1, &vertex_source, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATIOSIM_RESFAILED\n %s\n", infoLog);
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_source, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATIOSIM_RESFAILED\n %s\n", infoLog);
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n %s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // top right
        1.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // bottom left
        -1.0f, 1.0f, 0.0f, 0.0f, 0.0f   // top left
        // y texture coords are flipped
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3};

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // grid has to be initialized before we use it in the texture, it can be configured within the render loop
    // tweak diffusion and viscosity for different fluid behaviours
    // note that for larger time steps the simulation breaks (no idea why / might try to fix later)
    Fluid *grid = create_fluid_grid(SIM_RES, .005f, .0001f, .00004f);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUniform1i(glGetUniformLocation(shaderProgram, "texture"), 0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    double xpos0, ypos0;
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        // processInput(window);

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        xpos = clamp(xpos, 0., 800.);
        ypos = clamp(ypos, 0., 800.);
        if (xpos0 != xpos || ypos0 != ypos)
        {
            add_density(grid, xpos0 / (SCR_WIDTH + 1.0f) * SIM_RES, ypos0 / (SCR_HEIGHT + 1.0f) * SIM_RES, 1.4);
            add_velocity(grid, xpos0 / (SCR_WIDTH + 1.0f) * SIM_RES, ypos0 / (SCR_HEIGHT + 1.0f) * SIM_RES, (float)(xpos - xpos0) * 100, (float)(ypos - ypos0) * 100.);
            // printf("x: %.2f, y: %.2f\n", xpos, ypos);
            xpos0 = xpos;
            ypos0 = ypos;
        }

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, SIM_RES, SIM_RES, 0, GL_RED, GL_FLOAT, grid->density);
        fluid_grid_step(grid);
        int u_resLocation = glGetUniformLocation(shaderProgram, "u_resolution");
        glUseProgram(shaderProgram);
        glUniform2f(u_resLocation, SCR_WIDTH, SCR_HEIGHT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    free_fluid_grid(grid);
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    // do something
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
// void framebuffer_size_callback(GLFWwindow *window, int width, int height)
// {
//     // make sure the viewport matches the new window dimensions; note that width and
//     // height will be significantly larger than specified on retina displays.
//     glViewport(0, 0, width, height);
// }

double clamp(double x, double min, double max)
{
    if (x < min)
    {
        return min;
    }
    else if (x > max)
    {
        return max;
    }
    else
    {
        return x;
    }
}