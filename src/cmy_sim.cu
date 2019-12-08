#include <cmy_sim.h>
#include <cmy_contact_plane.h>

#ifdef GRAPHICS
#include <GLFW/glfw3.h>
#endif

CMySimulation::CMySimulation()
{
    sky_color = {1.0f, 1.0f, 1.0f, 0.0f};
}

void CMySimulation::setSkyColor(Color color ) {
    sky_color = color;
}

void CMySimulation::start() {
    if (ENDED) {
        throw std::runtime_error("The simulation has ended. Cannot call sim.start() after the end of the simulation.");
    }

    if (masses.size() == 0) {
        throw std::runtime_error("No masses have been added. Please add masses before starting the simulation.");
    }

    std::cout << "Starting simulation with " << masses.size() << " masses and " << springs.size() << " springs." << std::endl;

    RUNNING = true;
    STARTED = true;

    T = 0;

    if (this -> dt == 0.0) { // if dt hasn't been set by the user.
        dt = 0.01; // min delta

        for (Mass * m : masses) {
            if (m -> dt < dt)
                dt = m -> dt;
        }
    }

#ifdef GRAPHICS // SDL2 window needs to be created here for Mac OS
#ifdef SDL2
    createSDLWindow();
#endif
#endif

    updateCudaParameters();

    d_constraints.d_balls = thrust::raw_pointer_cast(&d_balls[0]);
    d_constraints.d_planes = thrust::raw_pointer_cast(&d_planes[0]);
    d_constraints.num_balls = d_balls.size();
    d_constraints.num_planes = d_planes.size();

    update_constraints = false;

//    cudaDeviceSetLimit(cudaLimitMallocHeapSize, 5 * (masses.size() * sizeof(CUDA_MASS) + springs.size() * sizeof(CUDA_SPRING)));
    toArray();

    d_mass = thrust::raw_pointer_cast(d_masses.data());
    d_spring = thrust::raw_pointer_cast(d_springs.data());

    gpu_thread = std::thread(&CMySimulation::_run, this);
}

void CMySimulation::_run() { // repeatedly start next
#ifdef GRAPHICS

#ifndef SDL2 // GLFW window needs to be created here for Windows
    createGLFWWindow();
#endif

#ifdef SDL2
    SDL_GL_MakeCurrent(window, context);
#endif
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);

    // Create and compile our GLSL program from the shaders
    this -> programID = LoadShaders(); // ("shaders/StandardShading.vertexshader", "shaders/StandardShading.fragmentshader"); //
    // Get a handle for our "MVP" uniform

    this -> MVP = getProjection(camera, looks_at, up); // compute perspective projection matrix

    this -> MatrixID = glGetUniformLocation(programID, "MVP"); // doesn't seem to be necessary

    generateBuffers(); // generate buffers for all masses and springs

    for (Constraint * c : constraints) { // generate buffers for constraint objects
        c -> generateBuffers();
    }

#endif
    
    execute();

    GPU_DONE = true;
}

void my_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void CMySimulation::createGLFWWindow() { // Copy from sim.cu and change sky color
    // Initialise GLFW
    if( !glfwInit() ) // TODO throw errors here
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    glfwSwapInterval(1);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1024, 768, "CUDA Physics Simulation", NULL, NULL);

    if (window == NULL) {
        fprintf(stderr,
                "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, my_framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);
    //    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        exit(1);
    }


    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark blue background
    glClearColor(sky_color.r, sky_color.g, sky_color.b, sky_color.a);
    
}

void CMySimulation::createPlane(const Vec & abc, double d ) { // creates half-space ax + by + cz < d
    if (ENDED) {
        throw std::runtime_error("The simulation has ended. New objects cannot be created.");
    }

    CMyContactPlane * new_plane = new CMyContactPlane(abc, d);
    constraints.push_back(new_plane);
    d_planes.push_back(CudaContactPlane(*new_plane));

    update_constraints = true;
}

Constraint* CMySimulation::lastConstraint() {
    return constraints.back();
}
