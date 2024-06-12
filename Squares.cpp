#include<iostream>
#include<string>
#include<stdlib.h>
#define SMALL_SQUARE_DIMENSION 112
#define BOARD_DIMENSION 896

using namespace std;
class Squares
{
    string piece_name_squares;
    string square_name;
    int X;
    int Y;
  public:
    Squares(){}
    Squares(int x,int y)
    {
        X=x;
        Y=y;
    }
    bool is_equal_coordinates(int x,int y)
    {
        if(x==X && y==Y)
            return true;
        return false;
    }
    void set_square_name(string temp)
    {
        square_name=temp;
    }
    void set_square_values(int x,int y)
    {
        X=x;
        Y=y;
    }
    int get_X()
    {
        return X;
    }
    int get_Y()
    {
        return Y;
    }
    bool is_clicked(int x,int y)
    {
        if((X<x) && (Y<y) && (x<(X+SMALL_SQUARE_DIMENSION)) && (y<(Y+SMALL_SQUARE_DIMENSION)))
            return true;
        else
            return false;
    }
    friend ostream & operator << (ostream &,Squares &a)
    {
        cout<<'('<<a.square_name<<','<<a.piece_name_squares<<','<<a.X<<','<<a.Y<<')'<<'\t';
        return cout;
    }
    void set_piece_name(string p_name)
    {
        piece_name_squares=p_name;
    }
    string return_piece_name()
    {
        return piece_name_squares;
    }
    string return_square_name()
    {
        return square_name;
    }
    void display()
    {
        cout<<'('<<square_name<<','<<piece_name_squares<<')'<<'\t';
    }
    void set_Null()
    {
        piece_name_squares="0";
        square_name="";       
    }
    void set_piece_Null()
    {
        piece_name_squares="";
    }
    bool operator ==(Squares &a)
    {
        if((piece_name_squares==a.piece_name_squares) && (square_name==a.square_name) && (X==a.X) && (Y==a.Y))
            return true;
        else
            return false;
    }
    bool is_empty()
    {
        if(piece_name_squares=="0")
            return true;
        return false;
    }
};