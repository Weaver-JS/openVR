OpenVR c++ implementation

ENG:
Prerequisites.
In order to make the project work you'll need to execute the project in x86 and have the dependencies cucaracha.bmp, SDL2.dll and shaders in the Debug folder. There's no initial need to do that because it should have the dependencies when you already download it.

Additional Information.
The engine is actually on development and it only renders a quad with a texture on it. It's an object oriented engine which uses the following most important classes:

Window -> It's the class that manages all about the SDL class and also its exceptions.

WindowManager-> It's a singleton class that manages all about the gameStates and the gameLoop. (And the exception that they could cause).

Drawable -> It's the parent class of all renderable class, It manages everything about passing information to openGL.

Camera-> It's all the information to create the viewMatrix and the methods to rotate and translate it.

Shader-> It manages everything about shaders, it load, compile and execute shader programs, and store all the information about it.

GameState-> It's a parent class that has the gameLoop events (init, update, destroy) (the gameloop which GameProgrammingPatterns talks about).

ESP:
Prerequisitos:
Para hacer que el proyecto funcione necesitas ejecutarlo en x86 (en la futura entrega se gestionará para x64) y tener las dependencias cucaracha.bmp, SDL2.dll and shaders en la carpeta Debug o Release según. No hay necesidad de hacer esto inicialmente debido a que debería de tener las dependencias una vez descargado.

Información adicional: 
El Engine esta en desarrollo actualmente y solamente renderiza un quad con una textura. Es un engine orientado a objetos que utiliza las siguientes clases más significativas en su pipeline:

Window -> Es una clase que se encarga de manejar todo lo referente a SDL incluyendo sus excepciones.

WindowManager-> Es una clase singleton que se encarga de manejar todo los gameStates y el gameLoop. 

Drawable -> Es la clase de la que todo objeto renderizable debería de heredar. Se encarga de manejar todo lo necesario para pasar información a openGL acerca de las primitivas 

Camera->Tiene toda la información matricial necesaria a la hora de enviar a openGL la matriz de view. También incluye métodos para trasladar y rotar la camara.

Shader-> Se encarga de todo lo referente a shaders, carga desde fichero, compila y ejecuta los shaders, guardando toda información acerca de ellos. 

GameState-> Es una clase destinada a herencia que tiene los eventos pilares del gameLoop (init,update,destroy).