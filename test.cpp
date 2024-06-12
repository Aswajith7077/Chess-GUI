#include<iostream>
//#include<SDL2/SDL.h>
class MyClass {
public:
    MyClass()
    {}
    MyClass(int x,int y)
    {
        x=x;y=y;
    }
private:
    int x,y;
};

int main() {
    //SDL_Window *window=SDL_CreateWindow("hello",SDL_WINPOS_UNDEFINED,SDL_WINPOS_UNDEFINED,800,600,SDL_WINDOW_ALLOW_HIGHDPI);
    const int ROWS = 5;
    const int COLS = 3;

    // Create a 2D array of MyClass objects
    MyClass myArray[ROWS][COLS];

    // Access and modify elements of the array
    myArray[0][0] = MyClass();  // Create a new object and assign it to the first element
    MyClass obj = myArray[2][1];  // Get the object at position (2, 1)

    return 0;
}
