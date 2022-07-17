#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);// 창의 크기를 조정할 때 호출되는 콜백 함수
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
/*GLSL언어로 작성됨.C와 유사하다. opengl과 버전을 맞춰야 하고, core profile기능을 사용할 것을 명시해야한다.
in 키워드를 사용해 모든 입력 정점 속성을 선언한다. 또한 layout으로 location을 0으로 설정한다.
main 함수 안의 gl_Position에 설정한 것을 vertex shader의 출력값으로 사용할 것이다. 입력값은 vec3이기에 vec4로 형변환 해준다.*/
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";
/*
* 픽셀의 출력 컬러 값을 계산하는 것에 관한 shader이다.
* 오직 하나의 출력변수를 필요로 하고, 그 변수는 크기가 4인 vector이다.
* out 키워드로 출력값을 선언한다. 여기서는 FragColor
* vec4의 네번째 파라미터는 알파값이고, 투명도를 조정한다.(1.0은 완전한 불투명)
*/

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();//glfw 초기화
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 첫 번째 파라미터 : 설정하고자 하는 옵션, 두 번째 파라미터 : 옵션의 값을 설정하는 변수
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    // GLAD가 OpenGL용 함수 포인터를 관리하기 때문에 OpenGL 함수들을 호출하기 전에 GLAD 초기화
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    //OpenGL이 shader를 사용하기 위해서는 런타임에 동적으로 shader 소스코드를 컴파일 해야한다.
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // 먼저 ID를 참조하여 shader 객체를 생성한다. 함수의 파라미터는 우리가 생성할 shader의 유형이다.
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    /*
    첫번째 파라미터:shader 객체
    두번째 파라미터:소스코드가 몇개의 문자열로 구성되었는가
    세번째 파라미터:vertex shader의 실제 소스코드
    네번째 파라미터:NULL
    */
    glCompileShader(vertexShader); //함수 호출 후 컴파일이 성공적으로 완료되었는지 확인할 수 있다.
    // check for shader compile errors
    int success;//성공을 나타내는 변수
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);//컴파일 성공했는지 확인하는 함수
    //위의 3줄로 컴파일 에러를 확인할 수 있다.
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);//실패했을 때 에러메시지 확인하는 함수
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    //vertex shader와 컴파일은 비슷하다.
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    //여기까지 두개의 shader가 컴파일 되었고 마지막 할 일은 shader program으로 두개의 shader 객체를 연결하는 것이다. shader 프로그램은 렌더링 할 때 우리가 사용할 수 있다.
    
    // link shaders
    unsigned int shaderProgram = glCreateProgram();//이 함수는 program을 생성하고 그 객체의 ID를 리턴한다.
    /*
    여러 shader를 결합한 마지막 연결된 버전.
    컴파일된 shader를 사용하기 위해 shader들을 shaderProgram 객체로 연결하고, 오브젝트를 렌더링 할 때 이 프로그램을 활성화 시키면 된다.
    활성화된 프로그램 안의 shader들은 렌더링 명령이 호출될 때 사용된다.
    */
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    //shader들을 프로그램에 붙이고 glLinkProgram을 사용하여 연결한다.
    
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);//shader 객체들을 program 객체로 연결하고 나면 shader 객체들은 더이상 필요 없기때문에 제거해준다.

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    //VBO : 많은 양의 정점들을 GPU 메모리 상에 저장할 수 있다. 대량의 데이터를 한꺼번에 그래픽카드로 전송할 수 있다는 장점이 있음
    //VAO : 하나 이상의 VBO를 담을 수 있는 객체, 개별 오브젝트의 모든 데이터를 담고 있고, 고유 ID를 가진다.
    //EBO : OpenGL이 어떠한 정점들을 그려야할 지 결정할 수 있는 인덱스들을 저장한다.정점의 중복으로 인한 오버헤드를 줄일 수 있다.
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); // 버퍼 ID를 생성하는 함수
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);//VAO를 바인딩, 그 후부터 해당 VBO와 속성 포인터를 바인딩/구성하고 VAO를 나중에 사용하기 위해 언바인딩 해야한다.

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // GL_ARRAY_BUFFER로 바인딩해주는 함수
    //위의 함수 이후로 우리가 호출하는 모든 버퍼는 현재 바인딩 된 버퍼(VBO)를 사용한다.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//미리 정의된 정점 데이터를 버퍼의 메모리에 복사한다.
    /*GL_STATIC_DRAW: 데이터가 거의 변하지 않습니다.
    GL_DYNAMIC_DRAW : 데이터가 자주 변경됩니다.
    GL_STREAM_DRAW : 데이터가 그려질때마다 변경됩니다.
    dynamic,stream으로 설정하면 그래픽카드가 빠르게 쓸 수 있는 메모리에 데이터를 저장한다.*/
    //여기까지, 정점 데이터를 그래픽카드의 메모리에 저장했고, 이 메모리는 VBO에 의해 관리된다.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    /*
    * OpenGL이 vertex 데이터를 어떻게 해석해야하는지 지정해준다.
    * 첫번째 파라미터:설정할 vertex 속성을 지정한다. vertex shader에서 layout 코드를 사용하여 position vertex 속성의 위치를 지정했던 것을 기억하자. vertex 속성의 위치를 0으로 설정하고 우리는 데이터를 이 vertex 속성에 전달하고자 하기 때문에 0을 전달한다.
    * 두번째 파라미터:vertex 속성의 크기를 저장한다.(vec3이므로 3)
    * 세번째 파라미터:데이터 타입을 지정한다.
    * 네번째 파라미터:데이터를 정규화할 것인지 지정한다. true로 하면 범위 밖에 있는 데이터를 -1과 1 사이의 값으로 매핑한다.
    * 다섯번째 파라미터:stride라고도 불리고, 연이은 vertex 속성 세트들 사이의 공백을 알려준다.
    * 여섯번째 파라미터:void* 타입이므로 형변환이 필요하다. 버퍼에서 데이터가 시작하는 위치의 offset이다. 위치 데이터가 데이터 배열의 시작 부분에 있기 때문에 0으로 지정한다.
    */

    /*
    * 각 vertex 속성은 VBO에 의해 관리되는 메모리로부터 데이터를 받는다.
    * glVertexAttribPointer 함수가 호출하기 전에 미리 정의된 VBO가 바인딩 되어 있으므로 vertex 속성 0이 해당 vertex 정점과 연결된다.
    */
    glEnableVertexAttribArray(0);//파라미터로 vertex 속성 location을 전달하고 호출하여 vertex 속성을 사용할 수 있도록 해야한다.

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) // loop마다 GLFW가 종료하도록 지시되었는지 확인
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // state-setting function
        glClear(GL_COLOR_BUFFER_BIT); // state-using function (어떤 색으로 지울것인지 검색하는 데에 현재 상태를 사용하기 때문)

        // draw our first triangle
        glUseProgram(shaderProgram);//링킹 결과는 program 객체이다. 이 객체는 함수의 파라미터로 들어간다.
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 3);//첫번쨰 : 그리려고 하는 유형, 두번째 : vertex 배열의 시작 인덱스, 세번째 : 몇개의 vertex를 그릴것인가
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//현재 바인딩 된 EBO의 인덱스들을 사용하여 그린다.
        // glBindVertexArray(0); // no need to unbind it every time 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window); // 컬러 버퍼를 교체
        glfwPollEvents(); // 이벤트가 발생했는지 확인, 윈도우 상태를 업데이트하며 정해진 함수 호출
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) // 렌더링 루프가 반복될 때 마다 호출한다.
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // 눌린 상태가 아니라면 GLFW_RELEASE를 리턴한다.
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);// 처음 두개 : 윈도우의 왼쪽 아래 모서리 위치 설정, 3,4번째 : 렌더링 윈도우의 너비와 높이를 픽셀로 설정
}
