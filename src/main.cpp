#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <cfloat> 

//librerias del grafo
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <random>
#include <queue>

//new
//librerias de opengl

#include <iostream>
#include <cmath>
#include <thread>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <random>


#include <map>
#include <set>
//estructura del grafo
using namespace std;

class Arista;



class Nodo {
public:
  int id;
  float x;
  float y;
  float radius;
  float distancia_euc;
  float distancia_euristica;
  float pesos_acumulados = 0.0;
  int distancia=0;
  int color_tomado = -1;
  vector<int> colores_disponibles = {0,1,2}; // siendo 0 = R, 1 = A , 2 = V

  vector<Arista*> aristas;
  //para graficar
  GLuint VAO,VBO;
  float color[3]; // Componentes RGB del color
  GLuint shaderProgram; // Programa de shaders
  // Constructor
  Nodo(float x, float y, float radius, float r, float g, float b ,int id,float distancia_euc,float distancia_eursitica) : id(id),x(x), y(y), radius(radius), VAO(0), VBO(0), shaderProgram(0),distancia_euc(distancia_euc),distancia_euristica(distancia_euristica) {
    color[0] = r / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
    color[1] = g / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
    color[2] = b / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
  }
  float heuristic(Nodo* goal) {
    return sqrt(pow(x - goal->x, 2) + pow(y - goal->y, 2));
  }
};
        

class Arista {
public:
  Nodo* origen; // Puntero al nodo origen de la arista
  Nodo* destino; // Puntero al nodo destino de la arista
  float peso; // Peso de la arista
  GLuint VAO_line; // ID del Vertex Array Object para la arista
  GLuint VBO_line; // ID del Vertex Buffer Object para la arista
  GLuint shaderProgram; // ID del programa de shaders para la arista
  float color[3]; // Componentes RGB del color de la arista

  // Constructor
  Arista(Nodo* origen, Nodo* destino, float peso, float r, float g, float b) :
    origen(origen), destino(destino), peso(peso), VAO_line(0), VBO_line(0), shaderProgram(0) {
    color[0] = r / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
    color[1] = g / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
    color[2] = b / 255.0f; // Convertir rango de 0-255 a 0.0-1.0

    origen->aristas.push_back(this);
    destino->aristas.push_back(this);
  }
};

class Rectangles{
  public:
  float x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4;

  unsigned int VAO_rect; // ID del Vertex Array Object para el rect
  unsigned int VBO_rect; // ID del Vertex Buffer Object para el rect
  float color[3]; // Componentes RGB del color del rect

  // Constructor
  Rectangles(float x1, float x2,float x3,float x4,
  float y1, float y2,float y3,float y4,
  float z1, float z2,float z3,float z4,
  float r, float g, float b) :
    x1(x1), x2(x2), x3(x3),x4(x4),
    y1(y1), y2(y2), y3(y3),y4(y4),
    z1(y1), z2(z2), z3(y3),z4(z4),

    VAO_rect(0), VBO_rect(0){
    color[0] = r / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
    color[1] = g / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
    color[2] = b / 255.0f; // Convertir rango de 0-255 a 0.0-1.0
  }
};

//compileShaders para nodos
void compileShadersNodo(Nodo& nodo) {
  const char* vertexShaderSource = R"glsl(
  #version 330 core
  layout (location = 0) in vec2 aPos;
  void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
  }
  )glsl";

  const char* fragmentShaderSource = R"glsl(
  #version 330 core
  out vec4 FragColor;
  uniform vec3 nodeColor; // Color del nodo
  void main() {
    FragColor = vec4(nodeColor, 1.0); // Usar el color del nodo
  }
  )glsl";

  GLuint vertexShader, fragmentShader;
  GLint success;
  GLchar infoLog[512];

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
  }

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
  }

  nodo.shaderProgram = glCreateProgram();
  glAttachShader(nodo.shaderProgram, vertexShader);
  glAttachShader(nodo.shaderProgram, fragmentShader);
  glLinkProgram(nodo.shaderProgram);
  glGetProgramiv(nodo.shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(nodo.shaderProgram, 512, NULL, infoLog);
    cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

//compileShaders para aristas
void compileShadersArista(Arista& arista) {
  const char* vertexShaderSource = R"glsl(
  #version 330 core
  layout (location = 0) in vec2 aPos;
  void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
  }
  )glsl";

  const char* fragmentShaderSource = R"glsl(
  #version 330 core
  out vec4 FragColor;
  uniform vec3 edgeColor; // Color de la arista
  void main() {
    FragColor = vec4(edgeColor, 1.0); // Usar el color de la arista
  }
  )glsl";

  GLuint vertexShader, fragmentShader;
  GLint success;
  GLchar infoLog[512];

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
  }

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
  }

  arista.shaderProgram = glCreateProgram();
  glAttachShader(arista.shaderProgram, vertexShader);
  glAttachShader(arista.shaderProgram, fragmentShader);
  glLinkProgram(arista.shaderProgram);
  glGetProgramiv(arista.shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(arista.shaderProgram, 512, NULL, infoLog);
    cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void createCircle(Nodo& nodo) {
  glGenVertexArrays(1, &nodo.VAO);
  glGenBuffers(1, &nodo.VBO);

  glBindVertexArray(nodo.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, nodo.VBO);

  float vertices[200];
  float radius = nodo.radius;
  for (int i = 0; i < 100; ++i) {
    float theta = 2.0f * 3.1415926f * float(i) / float(100);
    vertices[i * 2] = radius * cosf(theta) + nodo.x;
    vertices[i * 2 + 1] = radius * sinf(theta) + nodo.y;
  }

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void createEdge(Arista& arista) {
  glGenVertexArrays(1, &(arista.VAO_line));
  glGenBuffers(1, &(arista.VBO_line));

  glBindVertexArray(arista.VAO_line);
  glBindBuffer(GL_ARRAY_BUFFER, arista.VBO_line);

  float lineVertices[] = {
    arista.origen->x, arista.origen->y,
    arista.destino->x, arista.destino->y
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void createRectangles(Rectangles& rectangles) {
  float vertices[] = {
    rectangles.x3,rectangles.y3,rectangles.z3,
    rectangles.x4,rectangles.y4,rectangles.z4,
    rectangles.x2,rectangles.y2,rectangles.z2,
    rectangles.x1,rectangles.y1,rectangles.z1,
  };

  glGenVertexArrays(1, &rectangles.VAO_rect);
  glBindVertexArray(rectangles.VAO_rect);

  glGenBuffers(1, &rectangles.VBO_rect);
  glBindBuffer(GL_ARRAY_BUFFER, rectangles.VBO_rect);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}


class Grafo {
public:
  vector<Nodo*> nodos;
  vector<Arista*> aristas;
  int n;
  float posx, posy;

  Grafo(float posx_,float posy_, int n_) {
    posx = posx_;
    posy = posy_;
    n = n_;
    
  }

  ~Grafo() {
    for (Nodo* nodo : nodos)
      delete nodo;

    for (Arista* arista : aristas)
      delete arista;
  }

  void agregarNodo(float x, float y, float radius, float r, float g, float b ,int id) {
    nodos.push_back(new Nodo(x, y, radius, r, g, b,id,0.0,0.0));
  }

  void agregarArista(int idOrigen, int idDestino, float peso) { // Cambiado a float
    Nodo* origen = nullptr;
    Nodo* destino = nullptr;

    for (Nodo* nodo : nodos) {
      if (nodo->id == idOrigen)
        origen = nodo;
      if (nodo->id == idDestino)
        destino = nodo;
    }

    if (origen && destino) {
      Arista* x = new Arista(origen, destino, peso, 0, 128, 0);
      aristas.push_back(x);
    }
  }


  void crear_grafo_1(){
    for (int i = 1; i <= 4; i++)
    {
      agregarNodo(posx,posy,0.08f,255.0f,255.0f,255.0f,i);
      posx+=0.3;
      if (i % 2 == 0)
      {
        posy-= 0.3;
        posx-= 0.6f;
      }
    }
    // Conectar nodos con aristas
    for (int i = 0; i < n* n; i++) {
      int row = i / n ;
      int col = i % n ;

      // Conectar con nodo a la derecha
      if (col < n-1) {
        agregarArista(i+1, i + 1+1, 10);
      }

      // Conectar con nodo abajo
      if (row < n-1) {
        agregarArista(i+1, i + (n + 1), 10);

      }
    }
    posy+=0.45;
    posx = posx + 2*0.3f;
    agregarNodo(posx,posy,0.08f,255.0f,255.0f,255.0f,(2*n)+1);
    agregarArista(1, (2*n)+1, 10);
    agregarArista(2, (2*n)+1, 10);
    agregarArista(4, (2*n)+1, 10);
  }

  void crear_grafo_2(){
    for (int i = 1; i <= 6; i++)
    {
      agregarNodo(posx,posy,0.08f,255.0f,255.0f,255.0f,i);
      posx+=0.3;
      if (i % 3 == 0)
      {
        posy-= 0.3;
        posx-= 0.9f;
      }
    }
    // Conectar nodos con aristas
    for (int i = 0; i < n* n; i++) {
      int row = i / n ;
      int col = i % n ;

      // Conectar con nodo a la derecha
      if (col < n-1) {
        agregarArista(i+1, i + 1+1, 10);
      }

      // Conectar con nodo abajo
      if (row < n-1) {
        agregarArista(i+1, i + (n + 1), 10);

      }
    }
    posy+=0.45;
    posx = posx + 3*0.3f;
    agregarNodo(posx,posy,0.08f,255.0f,255.0f,255.0f,(2*n)+1);
    agregarArista(3, 7, 10);
    agregarArista(4, 7, 10);
    agregarArista(6, 7, 10);

    agregarArista(2, 6, 10);
  }
 

void ordenar_nodos_por_size_aristas()
{
  // Utilizamos una función lambda para comparar nodos por la cantidad de aristas
    sort(nodos.begin(), nodos.end(), [](const Nodo* a, const Nodo* b) {
    return a->aristas.size() > b->aristas.size();
    });
}

void ordenar_nodos_por_size_colores_disponibles()
{
  // Utilizamos una función lambda para comparar nodos por la cantidad de aristas
    sort(nodos.begin(), nodos.end(), [](const Nodo* a, const Nodo* b) {
    return a->colores_disponibles.size() < b->colores_disponibles.size();
    });
}
  void coloreo_mas_restringido(){

    
    ordenar_nodos_por_size_aristas();
    for(int i = 0; i<nodos.size();i++)
    {
      cout<<"\nID NODO: "<<nodos[i]->id<<" conexiones: "<<nodos[i]->aristas.size()<<endl;
      if(nodos[i]->colores_disponibles.size() == 0)
      {
        cout<<"NO HAY COLORES DISPONIBLES PARA EL ID: "<<nodos[i]->id<<endl;
        break;
      }
      nodos[i]->color_tomado = nodos[i]->colores_disponibles[0];
      
      for(int j = 0 ; j<nodos[i]->aristas.size();j++)
      {
        Nodo* neighbor = (nodos[i]->aristas[j]->origen == nodos[i]) ? nodos[i]->aristas[j]->destino : nodos[i]->aristas[j]->origen; // Obtener el vecino del nodo actual
        cout<<"Neighbor id:"<<neighbor->id<<endl;

        if(neighbor->colores_disponibles.size() != 0)
        {
          for(int k = 0; k < neighbor->colores_disponibles.size();k++)
          {
          cout<<"Colores disponibles: "<<neighbor->colores_disponibles[k]<<endl;
          if(nodos[i]->color_tomado == neighbor->colores_disponibles[k])
          {
            //cout<<"Color a eliminar: "<<nodos[i]->color_tomado<<endl;
            neighbor->colores_disponibles.erase(neighbor->colores_disponibles.begin()+k);
          }
            if(neighbor->colores_disponibles.size() == 0)
            {
              neighbor->colores_disponibles.push_back(nodos[i]->color_tomado);
              nodos[i]->color_tomado+=1;
            }
          }
          
        }
        else{
        cout<<"No hay colores0"<<endl;
        }
      }
      if(nodos[i]->color_tomado == 0)
      {
        nodos[i]->color[0] = 1.0f;
        nodos[i]->color[1] = 0.1f;
        nodos[i]->color[2] = 0.1f;
      }
      else if(nodos[i]->color_tomado == 1 )
      {
        nodos[i]->color[0] = 1.0f;
        nodos[i]->color[1] = 1.0f;
        nodos[i]->color[2] = 0.1f;
      }
      else if(nodos[i]->color_tomado == 2 )
      {
        nodos[i]->color[0] = 0.1f;
        nodos[i]->color[1] = 1.0f;
        nodos[i]->color[2] = 0.1f;
      }
    }
  }
  
  void coloreo_mas_restrictivo()
  {
    int todo_coloreado = 0;
    while(1)
    { int i = 0;
      cout<<"\nID NODO: "<<nodos[i]->id<<" conexiones: "<<nodos[i]->aristas.size()<<"Colores disponibles: "<<nodos[i]->colores_disponibles.size()<<endl;
      if(nodos[i]->colores_disponibles.size() == 0)
      {
        cout<<"NO HAY COLORES DISPONIBLES PARA EL ID: "<<nodos[i]->id<<endl;
        break;
      }
      nodos[i]->color_tomado = nodos[i]->colores_disponibles[0];
      
      for(int j = 0 ; j<nodos[i]->aristas.size();j++)
      {
        Nodo* neighbor = (nodos[i]->aristas[j]->origen == nodos[i]) ? nodos[i]->aristas[j]->destino : nodos[i]->aristas[j]->origen; // Obtener el vecino del nodo actual
        //cout<<"Neighbor id:"<<neighbor->id<<endl;

        if(neighbor->colores_disponibles.size() != 0)
        {
          for(int k = 0; k < neighbor->colores_disponibles.size();k++)
          {
          //cout<<"Colores disponibles: "<<neighbor->colores_disponibles[k]<<endl;
          if(nodos[i]->color_tomado == neighbor->colores_disponibles[k])
          {
            //cout<<"Color a eliminar: "<<nodos[i]->color_tomado<<endl;
            neighbor->colores_disponibles.erase(neighbor->colores_disponibles.begin()+k);
          }
            if(neighbor->colores_disponibles.size() == 0)
            {
              neighbor->colores_disponibles.push_back(nodos[i]->color_tomado);
              nodos[i]->color_tomado-=1;
              nodos[nodos.size()-2]->color_tomado+=1;
              nodos[nodos.size()-2]->color[0] = 0.1f;
              nodos[nodos.size()-2]->color[1] = 1.0f;
              nodos[nodos.size()-2]->color[2] = 0.1f;
              cout<<"No hay colores para vecino; "<<endl;
              break;
            }
          }
          
        }
        else{
        cout<<"No hay colores0"<<endl;
        }
      }
      if(nodos[i]->color_tomado == 0)
      {
        nodos[i]->color[0] = 1.0f;
        nodos[i]->color[1] = 0.1f;
        nodos[i]->color[2] = 0.1f;
      }
      else if(nodos[i]->color_tomado == 1 )
      {
        nodos[i]->color[0] = 1.0f;
        nodos[i]->color[1] = 1.0f;
        nodos[i]->color[2] = 0.1f;
      }
      else if(nodos[i]->color_tomado == 2 )
      {
        nodos[i]->color[0] = 0.1f;
        nodos[i]->color[1] = 1.0f;
        nodos[i]->color[2] = 0.1f;
      }
      nodos[i]->colores_disponibles.push_back(3);
      nodos[i]->colores_disponibles.push_back(4);
      nodos[i]->colores_disponibles.push_back(5);
      ordenar_nodos_por_size_colores_disponibles();
      for(int m = 0;m<nodos.size();m++)
      {
        if(nodos[m]->color_tomado != -1)
        {
          todo_coloreado = 1;
        }
        else{
          todo_coloreado = 0;
        }
      }
      if(todo_coloreado == 1)
      {
        break;
      }
    }
  }
  void borrarNodo(int idNodo) {
    // Buscar el nodo a eliminar
    Nodo* nodoEliminar = nullptr;
    for (Nodo* nodo : nodos) {
      if (nodo->id == idNodo) {
        nodoEliminar = nodo;
        break;
      }
    }

    if (!nodoEliminar)
      return;

    // Eliminar las aristas conectadas al nodo a eliminar y eliminarlas de VectorAristas
    for (Arista* arista : nodoEliminar->aristas) {
      auto& vecinoAristas = arista->destino == nodoEliminar ? arista->origen->aristas : arista->destino->aristas;
      vecinoAristas.erase(remove(vecinoAristas.begin(), vecinoAristas.end(), arista), vecinoAristas.end());
      aristas.erase(remove(aristas.begin(), aristas.end(), arista), aristas.end());
      delete arista;
    }

    // Eliminar el nodo
    nodos.erase(remove(nodos.begin(), nodos.end(), nodoEliminar), nodos.end());
    delete nodoEliminar;
  }

  void imprimir() {
    // Imprimir los nodos en orden
    for (int i = 0; i < nodos.size(); i++) {
      createCircle(*nodos[i]);
      compileShadersNodo(*nodos[i]);
      
    }

    // Imprimir las aristas
    for (int i = 0; i < aristas.size(); i++) {
      createEdge(*aristas[i]);
      compileShadersArista(*aristas[i]);
    }
  }

  void randomDelete(int a) {
    if (a < 0) a = 0;
    if (a > 100) a = 100;

    random_device rd;
    mt19937 g(rd());

    int numNodosEliminar = (a * nodos.size()) / 100;

    vector<int> indicesNodos(nodos.size());
      for (int i = 0; i < indicesNodos.size(); ++i) {
      indicesNodos[i] = i;
    }

    // Mezclar los índices de forma aleatoria
    shuffle(indicesNodos.begin(), indicesNodos.end(), g);

    // Lista temporal para almacenar los nodos a eliminar
    vector<Nodo*> nodosAEliminar;

    // Agregar los nodos a eliminar a la lista temporal
    for (int i = 0; i < numNodosEliminar; ++i) {
      int idNodoEliminar = nodos[indicesNodos[i]]->id;
      for (auto iter = nodos.begin(); iter != nodos.end(); ++iter) {
        if ((*iter)->id == idNodoEliminar) {
          nodosAEliminar.push_back(*iter);
          break;
        }
      }
    }

    for (auto nodo : nodosAEliminar) {
      borrarNodo(nodo->id);
    }
  }
};

//estructura del graficador
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// variables globales
Grafo G(-0.85f,0.8,2);
Grafo G_2(-0.85f,0,3);
vector<Nodo*> inicio_fin;

vector<Nodo*> OrdenarNodos(vector<Nodo*>& vector_Ord)
{
  // Ordenar el vector de nodos según el peso
  sort(vector_Ord.begin(), vector_Ord.end(), [](Nodo* a, Nodo* b) {
    for(int i =0;i<a->aristas.size();i++)
    {
      if(a->aristas[i]->destino == inicio_fin[0])
      {
        for(int j = 0;j<b->aristas.size();j++)
        {
          if(b->aristas[j]->destino == inicio_fin[0])
          {
              return a->aristas[i]->peso < b->aristas[j]->peso; // Comparar por peso de la primera arista de cada nodo destino
          }
        }
      }
    }
    //warning pero nunca llega
    return a->aristas[0]->peso < b->aristas[0]->peso; // Comparar por peso de la primera arista de cada nodo destino
});
    return vector_Ord;
}

vector<Nodo*> nodos_cercanos(vector<Nodo*>& vector_cercanos,Nodo nodo_central)
{
  for(int i = 0;i<nodo_central.aristas.size();i++)
  {
    vector_cercanos.push_back(nodo_central.aristas[i]->destino);
    //cout<<"Id nodos colindantes: "<<nodo_central.aristas[i]->destino->id<<endl;
  }
  return vector_cercanos;
}


float calcularDistanciaEuc(Nodo* a, Nodo* b) {
  float dx = a->x - b->x;
  float dy = a->y - b->y;
  return sqrt(dx * dx + dy * dy);
}

float calcularDistanciaHeuristica(Nodo* a, Nodo* b) {
  return calcularDistanciaEuc(a, b);
}

void resetGrafo () {
  for (Nodo* nodo : G.nodos) {
    nodo->color[0] = 1.0f;
    nodo->color[1] = 1.0f;
    nodo->color[2] = 1.0f;
    nodo->distancia_euc = 0.0;
    nodo->distancia_euristica = 0.0;
    nodo->pesos_acumulados = 0.0;
    nodo->distancia = 0;
    nodo->colores_disponibles.clear();
    nodo->colores_disponibles.push_back(2);
    nodo->colores_disponibles.push_back(1);
    nodo->colores_disponibles.push_back(0);
    nodo->color_tomado = -1;
    //nodo->colores_disponibles = {0,1,2}; // siendo 0 = R, 1 = A , 2 = V
    inicio_fin.clear();
  }
  for (Nodo* nodo : G_2.nodos) {
    nodo->color[0] = 1.0f;
    nodo->color[1] = 1.0f;
    nodo->color[2] = 1.0f;
    nodo->distancia_euc = 0.0;
    nodo->distancia_euristica = 0.0;
    nodo->pesos_acumulados = 0.0;
    nodo->distancia = 0;
    nodo->colores_disponibles.clear();
    nodo->colores_disponibles.push_back(0);
    nodo->colores_disponibles.push_back(1);
    nodo->colores_disponibles.push_back(2);
    nodo->color_tomado = -1;
    inicio_fin.clear();
  }
}
// Función para manejar los eventos de click
void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
  // Verificar si el botón izquierdo del mouse ha sido presionado
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Imprimir las coordenadas del click
    // cout << "Click en coordenadas (" << xpos << ", " << ypos << ")" << endl;
    // Obtener las coordenadas normalizadas del cursor del mouse
    float normalizedX = (xpos / 800) * 2 - 1; // Convertir de rango [0, screenWidth] a [-1, 1]
    float normalizedY =  1 - ((ypos/800)*2); // Convertir de rango [0, screenHeight] a [-1, 1]
    // cout << "Click en coordenadas normalizadas (" << normalizedX << ", " << normalizedY << ")" << endl;

    // cout << "Coordenadas del primer Nodo(" << G.nodos[0]->x << ", " << G.nodos[0]->y << ")" << endl;
    for (int i = 0; i<G.nodos.size() ; i++) {
      if (pow(normalizedX - G.nodos[i]->x, 2) + pow(normalizedY - G.nodos[i]->y, 2) <= pow(G.nodos[i]->radius, 2)) {
        // Cambiar el color del nodo encontrado a rojo
        G.nodos[i]->color[0] = 0;
        G.nodos[i]->color[1] = 255;
        G.nodos[i]->color[2] = 0;
        // cout<<"Nodo encontrado: "<<G.nodos[i]->id<<endl;
        // cout<<"Nodo encontrado pos x: "<<G.nodos[i]->x<<endl;
        // cout<<"Nodo encontrado pos y: "<<G.nodos[i]->y<<endl;
        inicio_fin.push_back(G.nodos[i]);
        break; // Salir del bucle al encontrar el nodo
      }
    }

    //G_2
    for (int i = 0; i<G_2.nodos.size() ; i++) {
      if (pow(normalizedX - G_2.nodos[i]->x, 2) + pow(normalizedY - G_2.nodos[i]->y, 2) <= pow(G_2.nodos[i]->radius, 2)) {
        // Cambiar el color del nodo encontrado a rojo
        G_2.nodos[i]->color[0] = 0;
        G_2.nodos[i]->color[1] = 255;
        G_2.nodos[i]->color[2] = 0;
        // cout<<"Nodo encontrado: "<<G.nodos[i]->id<<endl;
        // cout<<"Nodo encontrado pos x: "<<G.nodos[i]->x<<endl;
        // cout<<"Nodo encontrado pos y: "<<G.nodos[i]->y<<endl;
        inicio_fin.push_back(G_2.nodos[i]);
        break; // Salir del bucle al encontrar el nodo
      }
    }
    if (normalizedX > 0.7f && normalizedX <0.99f  && normalizedY < 0.88f && normalizedY > 0.75f)
    {           
      G.coloreo_mas_restringido();
    }
    if (normalizedX > 0.7f && normalizedX <0.99f  && normalizedY < 0.68f && normalizedY > 0.55f)
    {           
      G.coloreo_mas_restrictivo();
    }
    if (normalizedX > 0.7f && normalizedX <0.99f  && normalizedY < 0.48f && normalizedY > 0.35f)
    {           
      
      G_2.coloreo_mas_restringido();
    }

    if (normalizedX > 0.7f && normalizedX <0.99f  && normalizedY < 0.28f && normalizedY > 0.15f)
    {
       G_2.coloreo_mas_restrictivo();
    }

    if (normalizedX > 0.7f && normalizedX <0.99f  && normalizedY < 0.08f && normalizedY > -0.05f)
    {           
      //REFRESH
      resetGrafo();
    }
  }
}

int main() {
  //main del graficador

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 800, "Grafo", NULL, NULL);
  if (window == NULL) {
    cout << "Failed to create GLFW Window" << endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  //funcion del mouse
  glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
  mouse_callback(window, button, action, mods);
  });

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    cout << "Failed to initialize GLAD" << endl;
    return -1;
  }
  //termina de crear la pantalla 

  //glViewport(0, 0, 800, 800);
  //glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

  //main del grafo
  cout << endl;
  //cout << (G.nodos[0])->aristas[0]->peso;
 
 
  G.crear_grafo_1();
  G_2.crear_grafo_2();
  G.imprimir();
  G_2.imprimir();

  
  vector<Rectangles*> algoritmos_mostrar;
  float x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4;
  x1 =0.7f;
  y1 = 0.88f;
  z1 = 0.0f;

  x2 = 0.99f;
  y2 = 0.88f;
  z2 = 0.0f;

  x3 = 0.7f;
  y3 = 0.75f;
  z3 = 0.0f;

  x4 =  0.99f;
  y4 = 0.75f;
  z4 = 0.0f;
  for(int i = 0; i <5;i++)
  {
    algoritmos_mostrar.push_back(new Rectangles(x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4,0,255,0));
    createRectangles(*algoritmos_mostrar[i]);
    y1-=0.2f;
    y2-=0.2f;
    y3-=0.2f;
    y4-=0.2f;
  }


  
  //cout<<"GRAFO ACTUALIZADO"<<endl;

  while (!glfwWindowShouldClose(window)) {
    // Procesar eventos de la ventana
    glfwPollEvents();
    processInput(window);

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    // Imprimir los nodos en orden de G
    for (int i = 0; i < G.nodos.size(); i++) {
      //cout << G.nodos[i]->id << "\t";
      glUseProgram(G.nodos[i]->shaderProgram);
      // Pasa el color del nodo al shader fragment
      int nodeColorLocation = glGetUniformLocation(G.nodos[i]->shaderProgram, "nodeColor");
      glUniform3fv(nodeColorLocation, 1, G.nodos[i]->color);
      glBindVertexArray(G.nodos[i]->VAO);
      glDrawArrays(GL_TRIANGLE_FAN, 0, 100);
    }
    // Dibujar aristas
    // Imprimir las aristas
    for (int i = 0; i < G.aristas.size(); i++) {
      glUseProgram(G.aristas[i]->shaderProgram);
      int edgeColorLocation = glGetUniformLocation(G.aristas[i]->shaderProgram, "edgeColor");
      glUniform3fv(edgeColorLocation, 1, G.aristas[i]->color);
      glBindVertexArray(G.aristas[i]->VAO_line);
      glDrawArrays(GL_LINES, 0, 2);
    }



    // Imprimir los nodos en orden de G_2
    for (int i = 0; i < G_2.nodos.size(); i++) {
      //cout << G.nodos[i]->id << "\t";
      glUseProgram(G_2.nodos[i]->shaderProgram);
      // Pasa el color del nodo al shader fragment
      int nodeColorLocation = glGetUniformLocation(G_2.nodos[i]->shaderProgram, "nodeColor");
      glUniform3fv(nodeColorLocation, 1, G_2.nodos[i]->color);
      glBindVertexArray(G_2.nodos[i]->VAO);
      glDrawArrays(GL_TRIANGLE_FAN, 0, 100);
    }
    // Dibujar aristas
    // Imprimir las aristas
    for (int i = 0; i < G_2.aristas.size(); i++) {
      glUseProgram(G_2.aristas[i]->shaderProgram);
      int edgeColorLocation = glGetUniformLocation(G_2.aristas[i]->shaderProgram, "edgeColor");
      glUniform3fv(edgeColorLocation, 1, G_2.aristas[i]->color);
      glBindVertexArray(G_2.aristas[i]->VAO_line);
      glDrawArrays(GL_LINES, 0, 2);
    }

    //dibuja rectangulos

    for(int i = 0; i<algoritmos_mostrar.size();i++)
    {
      glBindVertexArray(algoritmos_mostrar[i]->VAO_rect);
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Modificado para dibujar un rectángulo
    }
    glfwSwapBuffers(window);
  }


  //liberar memoria
  for (int i = 0; i < G.nodos.size(); i++) {
    glDeleteVertexArrays(1, &(G.nodos[i])->VAO);
    glDeleteBuffers(1, &(G.nodos[i])->VBO);
    glDeleteProgram(G.nodos[i]->shaderProgram);
  }
  for (int i = 0; i < G.aristas.size(); i++) {
    glDeleteVertexArrays(1, &(G.aristas[i])->VAO_line);
    glDeleteBuffers(1, &(G.aristas[i])->VBO_line);
    glDeleteProgram(G.aristas[i]->shaderProgram);
  }
  

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}