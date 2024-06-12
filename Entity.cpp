#include<iostream>
#include<string>
using namespace std;
class Entity
{
    string piece_name,color;
    int direction;
  public:
    Entity(string s="pawn",string c="white")
    {
        piece_name=s;
    }
    void check_action()
    {
        if(piece_name=="pawn"&&  color=="white")
        {
            direction=1;
        }
        else if(piece_name=="pawn" && color=="white")
        {
            direction=-1;
        }
    }
}