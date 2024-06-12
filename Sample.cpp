#include<iostream>
#include<sstream>
#include<string.h>
#include<vector>
#include<cstring>
#include<string>
#include<ctype.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL_mixer.h>

#include"Squares.cpp"

#define X_UPPER 1336
#define Y_UPPER 926
#define X_LOWER 440
#define Y_LOWER 30

#define SMALL_SQUARE_DIMENSION 112
#define BOARD_DIMENSION 896
//defining const file paths [Black]
const char *black_pawn="res/img/Black Pawn.png";
const char *black_rook="res/img/Black Rook.png";
const char *black_knight="res/img/Black Knight.png";
const char *black_bishop="res/img/Black Bishop.png";
const char *black_queen="res/img/Black Queen.png";
const char *black_king="res/img/Black King.png";

//defining const file paths [White]
const char *white_pawn="res/img/White Pawn.png";
const char *white_rook="res/img/White Rook.png";
const char *white_knight="res/img/White Knight.png";
const char *white_bishop="res/img/White Bishop.png";
const char *white_queen="res/img/White Queen.png";
const char *white_king="res/img/White King.png";

const char *Target="res/img/Target.png";
const char *Aim="res/img/Aim.png";

const char *start_game_name="res/music/Start_Game.mp3";
const char *end_game_name="res/music/End_Game.mp3";
const char *move_name="res/music/Move.mp3";
const char *capture_name="res/music/Capture.mp3";
const char *castling_name="res/music/Castling.mp3";
const char *check_name="res/music/Check.mp3";
const char *check_mate_name="res/music/Check_Mate.mp3";
const char *error_name="res/music/Error.mp3";

using namespace std;
const int WIDTH = 1800, HEIGHT =960;

string a[][8]={"r1","n1","b1","q1","k","b2","n2","r2",
               "p1","p2","p3","p4","p5","p6","p7","p8",
               "0","0","0","0","0","0","0","0",
               "0","0","0","0","0","0","0","0",
               "0","0","0","0","0","0","0","0",
               "0","0","0","0","0","0","0","0",
               "P1","P2","P3","P4","P5","P6","P7","P8",
               "R1","N1","B1","Q1","K","B2","N2","R2"};
string square_names[][8]={};
string alpha="abcdefgh";

class Pawn;
class Rook;
class Bishop;
class Knight;
class Queen;
class King;

SDL_Texture *aim,*target;

SDL_Texture *IMG_LoadTexture(SDL_Renderer *renderer, const char *file)
{
    SDL_Texture *texture = NULL;
    SDL_Surface *surface = IMG_Load(file);
    if (surface) {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
    return texture;
}
bool is_outer(int x,int y)
{
    if(x<X_LOWER || x>X_UPPER || y<Y_LOWER || y>Y_UPPER)
        return true;
    else
        return false;
}

Squares SQ[8][8];
void SET_SQUARES()
{
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            string temp;
            stringstream s;
            s<<8-i;
            s>>temp;
            SQ[i][j]=Squares();
            SQ[i][j].set_square_name(alpha[j]+temp);
            SQ[i][j].set_piece_name(a[i][j]);
            cout<<a[i][j]<<'\t';
            SQ[i][j].set_square_values(440+SMALL_SQUARE_DIMENSION*j,30+SMALL_SQUARE_DIMENSION*i);
        }
        cout<<endl;
    }
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
            SQ[i][j].display();
        cout<<endl;
    }
}

SDL_Window *window;
class Render_Window
{
    SDL_Renderer *renderer;
  public:
    void important()
    {
        int check=SDL_Init(SDL_INIT_EVERYTHING);
        if(check<0)
            cout<<"Cannot initialize SDL_Init : "<<SDL_GetError()<<endl;
        window = SDL_CreateWindow("Chess",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WIDTH,HEIGHT,SDL_WINDOW_ALLOW_HIGHDPI);
        if(window==NULL)
            cout<<"Could not create window : "<<SDL_GetError()<<endl;
        
        renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
        SDL_SetRenderDrawColor(renderer,0,95,0,255);
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer); 
    }
    Render_Window(int set=0)
    {
        if(set!=0) 
            important();
    }
    
    void present_renderer()
    {
        SDL_RenderPresent(renderer);
    }
    SDL_Renderer * get_renderer()
    {
        return renderer;
    }
    void clear_renderer()
    {
        SDL_RenderClear(renderer);
    }
    ~Render_Window()
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }
};
Render_Window render(1);
Render_Window basic_render=render;

struct Current_Position
{
    int X,Y;
    int H,W;
};

class Piece
{
  protected:
    int sq_i,sq_j;
    string piece_name;
    string current_square;
    char *file_name;
    SDL_Texture *texture;
    SDL_Rect source,destination;
  public:
    struct Current_Position CP;
  public:
    Piece()
    {
        /*piece_name="";
        texture=NULL;
        file_name=NULL;
        current_square="";*/
    }
    Piece(string p,const char * s,string square_name)
    {
        piece_name=p;
        current_square=square_name;
        file_name=new char[strlen(s)+1];
        strcpy(file_name,s);
        texture=IMG_LoadTexture(render.get_renderer(),file_name);
        if(texture==NULL)
        {
            cout<<"Failed to load "<<file_name<<" : "<<IMG_GetError()<<' '<<SDL_GetError()<<endl;
        }
    }
    void set_square_name(string s)
    {
        current_square=s;
    }
    void set_sq_i_and_j(int t_i,int t_j)
    {
        sq_i=t_i;
        sq_j=t_j;
    }
    friend ostream & operator << (ostream &,Piece &a)
    {
        cout<<a.piece_name<<'\t';
        return cout;
    }
    void piece_value(string p,const char *s,string s_name)
    {
        piece_name=p;
        current_square=s_name;
        file_name=new char[strlen(s)+1];
        strcpy(file_name,s);
        texture=IMG_LoadTexture(render.get_renderer(),file_name);
        if(texture==NULL)
        {
            cout<<"Failed to load "<<file_name<<" : "<<IMG_GetError()<<endl;
        }
    }
    void set_destination_value(int x,int y)
    {
        render.clear_renderer();
        destination.x=x;
        destination.y=y;
        SDL_RenderCopy(render.get_renderer(),texture,NULL,&destination);  
        render.present_renderer();
    }
    void create_texture(int x,int y,int w,int h)
    { 
	    source.x = 0;
	    source.y =0;
    	source.w;
	    source.h;
	    SDL_QueryTexture(texture, NULL, NULL, &source.w, &source.h);
	    destination.x = x;
	    destination.y = y;
	    destination.w = w;
	    destination.h = h;

        CP.X=x;
        CP.Y=y;
        CP.H=h;
        CP.W=w;

        SDL_RenderCopy(render.get_renderer(),texture,&source,&destination);   
        SDL_RenderPresent(render.get_renderer());
    }
    void create_texture(SDL_Texture *t,int x,int y,int w,int h)
    {
        source.x = 0;
	    source.y =0;
    	source.w;
	    source.h;
	    SDL_QueryTexture(texture, NULL, NULL, &source.w, &source.h);
	    destination.x = x;
	    destination.y = y;
	    destination.w = w;
	    destination.h = h;

        CP.X=x;
        CP.Y=y;
        CP.H=h;
        CP.W=w;

        SDL_RenderCopy(render.get_renderer(),t,&source,&destination);   
        SDL_RenderPresent(render.get_renderer());
    }
    void show_aim(Squares &a)
    {
        SDL_Texture *aim=IMG_LoadTexture(render.get_renderer(),Aim);
        SDL_Rect destination;
        cout<<'('<<a.get_X()<<','<<a.get_Y()<<')'<<endl;
        destination.x = a.get_X()+34;
        destination.y = a.get_Y()+34;
        destination.w = 45;
        destination.h = 45;
        SDL_RenderCopy(render.get_renderer(),aim,NULL,&destination);  
        SDL_RenderPresent(render.get_renderer());
    }
    /*
    void show_target(Squares &a)
    {
        SDL_Texture *target=IMG_LoadTexture(render.get_renderer(),Target);
        SDL_Rect destination;
        cout<<'('<<a.get_X()<<','<<a.get_Y()<<')'<<endl;
        destination.x = a.get_X()+34;
        destination.y = a.get_Y()+34;
        destination.w = 45;
        destination.h = 45;
        SDL_RenderCopy(render.get_renderer(),aim,NULL,&destination);  
        SDL_RenderPresent(render.get_renderer());
    }*/
    
    struct Current_Position get_CP()
    {
        return CP;
    }
    void find(Squares &a)
    {
        bool c=true;
        for(int i=0;i<8;i++)
        {
            for(int j=0;j<8;j++)
            {
                
                if(SQ[i][j].is_equal_coordinates(a.get_X(),a.get_Y()))
                {
                    a.set_piece_name(SQ[i][j].return_piece_name());
                    a.set_square_name(SQ[i][j].return_square_name());
                    c=false;
                    break;
                }
            }
            if(c==false)
                break;
        }
    }
    void display()
    {
        cout<<"("<<current_square<<','<<piece_name<<','<<CP.X<<','<<CP.Y<<")"<<'\t';
    }
    string get_piece_name()
    {
        return piece_name;
    }
    void destroy_texture()
    {
        SDL_DestroyTexture(texture);
    }
    int get_sq_i()
    {
        return sq_i;
    }
    int get_sq_j()
    {
        return sq_j;
    }
    bool is_not_Null()
    {
        if( piece_name =="" || texture==NULL || file_name==NULL || current_square=="")
            return false;
        return true;

    }
    void set_null()
    {
        piece_name="0";
        current_square="";
        sq_i=-100;
        sq_j=-100;
    }
    ~Piece()
    {
        SDL_DestroyTexture(texture);
    }
};

//Pawn class 

class Pawn:public Piece
{
    int case_;
    int initial_move;
    int move;
    Squares double_forward;
    Squares forward;
    Squares right_diag;
    Squares left_diag;
  public:
    Pawn()
    {
        case_=0;
    }
    void display()
    {
        cout<<"U and V : "<<sq_i<<' '<<sq_j<<endl;
        cout<<"That square : "<<SQ[sq_i][sq_j]<<endl;
        cout<<"Piece name : "<<piece_name<<endl;
        cout<<"CP.X and CP.Y : "<<CP.X<<' '<<CP.Y<<' '<<CP.H<<' '<<CP.W<<endl;
    }
    Pawn(string p,const char *s,string s_name)
    :Piece(p,s,s_name){}
    void print_render(int x,int y)
    {
        CP.X=x;
        CP.Y=y;
        create_texture(CP.X+19,CP.Y+18,75,75);
    }
    bool is_friend(string a)
    {
        if(isupper(a[0]) && isupper(piece_name[0]))
            return true;
        else if(islower(a[0]) && islower(piece_name[0]))
            return true;
        else 
            return false;
    }
    bool is_double_forward_valid()
    {
        if(initial_move==0)
            return true;
        return false;
    }
    Squares & get_for()
    {
        return forward;
    }
    Squares & get_double_for()
    {
        return double_forward;
    }
    Squares & get_left_diag()
    {
        return left_diag;
    }
    Squares & get_right_diag()
    {
        return right_diag;
    }
    void check_white(int x,int y)
    {
        cout<<"Count : "<<initial_move<<endl;
        forward.set_square_values(x,y-SMALL_SQUARE_DIMENSION);
        right_diag.set_square_values(x+SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION);
        left_diag.set_square_values(x-SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION);
        find(forward);
        find(right_diag);
        find(left_diag);
        string t1=right_diag.return_piece_name(),t2=left_diag.return_piece_name();
        if(initial_move==0)
        {
            case_=0;
            double_forward.set_square_values(x,y-2*SMALL_SQUARE_DIMENSION);
            cout<<"Inside the double function WHITE "<<initial_move<<endl;
            find(double_forward);
            show_aim(double_forward);
            initial_move=1;
            cout<<"After change : "<<initial_move<<endl;
        }
        if(forward.return_piece_name()=="0")
        {
            case_=1;
            show_aim(forward);
        }
        if(t1[0]!='0'  && islower(char(t1[0])) )
        {
            case_=2;
            show_aim(right_diag);
        }
        if( t2[0]!='0' && islower(char(t2[0])))
        {
            case_=3;
            show_aim(left_diag);
        }
    }
    void check_black(int x,int y)
    {
        forward.set_square_values(x,y+SMALL_SQUARE_DIMENSION);
        right_diag.set_square_values(x-SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION);
        left_diag.set_square_values(x+SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION);
        find(forward);
        find(right_diag);
        find(left_diag);
        string t1=right_diag.return_piece_name(),t2=left_diag.return_piece_name();
        if(initial_move==0)
        {
            case_=0;
            cout<<"Inside the double function BLACK "<<initial_move<<endl;
            double_forward.set_square_values(x,y+2*SMALL_SQUARE_DIMENSION);
            find(double_forward);
            show_aim(double_forward);
            initial_move=1;
            cout<<"After change : "<<initial_move<<endl;
        }
        if(forward.return_piece_name()=="0")
        {
            case_=1;
            show_aim(forward);
        }
        if(t1[0]!='0'  && isupper(char(t1[0])) && t1[0]!='K')
        {
            case_=2;
            show_aim(right_diag);
        }
        if( t2[0]!='0' && isupper(char(t2[0])) && t2[0]!='K')
        {
            case_=3;
            show_aim(left_diag);
        }
    }
    bool is_left_valid(Squares &a)
    {
        if(!left_diag.is_empty())
            return true;
        else
            return false; 
    }
    bool is_right_valid(Squares &a)
    {
        if(!right_diag.is_empty())
            return true;
        else
            return false; 
    }
    bool check_square(Squares &a)
    {
        cout<<a<<' '<<left_diag<<endl;
        if(a==left_diag || a==right_diag)
            return true;
        else 
            return false;
    }
    bool is_for_valid(Squares &a)
    {
        if(a.is_empty() && forward.is_empty())
            return true;
        else
            return false;
    }
    void set_null()
    {
        Piece::set_null();
        double_forward.set_Null();
        forward.set_Null();
        left_diag.set_Null();
        right_diag.set_Null();
    }
};
class Rook:public Piece
{
    int move;
    vector<Squares> right;
    vector<Squares> left;
    vector<Squares> top;
    vector<Squares> bottom;
  public:
    Rook(){}
    Rook(string p,const char *s,string s_name)
    :Piece(p,s,s_name)
    {}
    void display()
    {
        cout<<"U and V : "<<sq_i<<' '<<sq_j<<endl;
        cout<<"That square : "<<SQ[sq_i][sq_j]<<endl;
        cout<<"Piece name : "<<piece_name<<endl;
        cout<<"CP.X and CP.Y : "<<CP.X<<' '<<CP.Y<<' '<<CP.H<<' '<<CP.W<<endl;
    }
    void print_render(int x,int y)
    {
        CP.X=x;
        CP.Y=y;
        create_texture(CP.X+17,CP.Y+16,75,90);
    }
    bool outer(int x,int y)
    {
        if(x<440 || y<30 || x>1224 || y>814)
            return true;
        else
            return false;
    }
    bool is_friend(string a)
    {
        if(isupper(a[0]) && isupper(piece_name[0]))
            return true;
        else if(islower(a[0]) && islower(piece_name[0]))
            return true;
        else 
            return false;
    }
    void check_move(int x,int y)
    {
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x+(i+1)*SMALL_SQUARE_DIMENSION;
            v=y;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    right.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    right.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
            else
                break;
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x-(i+1)*SMALL_SQUARE_DIMENSION;
            v=y;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    left.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    left.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x;
            v=y+(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    bottom.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    bottom.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x;
            v=y-(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    top.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    top.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
    }
    bool check_square(Squares &a)
    {
        for(vector<Squares>::iterator i=right.begin();i!=right.end();i++)
        {
            if(*i==a)
                return true;
        }
        for(vector<Squares>::iterator i=left.begin();i!=left.end();i++)
        {
            if(*i==a)
                return true;
        }
        for(vector<Squares>::iterator i=bottom.begin();i!=bottom.end();i++)
        {
            if(*i==a)
                return true;
        }
        for(vector<Squares>::iterator i=top.begin();i!=top.end();i++)
        {
            if(*i==a)
                return true;
        }
        return false;
    }

};
class Queen:public Piece
{
    int move;
    static int n;
    vector<Squares> right_top,left_top;
    vector<Squares> left_bottom,right_bottom;
    vector<Squares> right,left;
    vector<Squares> top,bottom; 
 public:
    Queen(){}
    Queen(string p,const char *s,string s_name,int x,int y)
    :Piece(p,s,s_name)
    {
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x+(i+1)*SMALL_SQUARE_DIMENSION;
            v=y-(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    right_top.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    right_top.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
            else
                break;
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x-(i+1)*SMALL_SQUARE_DIMENSION;
            v=y-(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    left_top.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    left_top.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x-(i+1)*SMALL_SQUARE_DIMENSION;
            v=y+(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    left_bottom.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    left_bottom.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x+(i+1)*SMALL_SQUARE_DIMENSION;
            v=y+(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    right_bottom.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    right_bottom.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x+(i+1)*SMALL_SQUARE_DIMENSION;
            v=y;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    right.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    right.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
            else
                break;
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x-(i+1)*SMALL_SQUARE_DIMENSION;
            v=y;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    left.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    left.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x;
            v=y+(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    bottom.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    bottom.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x;
            v=y-(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    top.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    top.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
    }
    void display()
    {
        cout<<"U and V : "<<sq_i<<' '<<sq_j<<endl;
        cout<<"That square : "<<SQ[sq_i][sq_j]<<endl;
        cout<<"Piece name : "<<piece_name<<endl;
        cout<<"CP.X and CP.Y : "<<CP.X<<' '<<CP.Y<<' '<<CP.H<<' '<<CP.W<<endl;
    }
    void print_render(int x,int y)
    {
        CP.X=x;
        CP.Y=y;
        create_texture(x+9,y+16,95,90);
    }
    bool outer(int x,int y)
    {
        if(x<440 || y<30 || x>1224 || y>814)
            return true;
        else
            return false;
    }
    static int inc()
    {
        n+=1;
        return n;
    }
    bool is_friend(string a)
    {
        if(isupper(a[0]) && isupper(piece_name[0]))
            return true;
        else if(islower(a[0]) && islower(piece_name[0]))
            return true;
        else 
            return false;
    }
    void check_move(int x,int y)
    {
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x+(i+1)*SMALL_SQUARE_DIMENSION;
            v=y-(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    right_top.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    right_top.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
            else
                break;
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x-(i+1)*SMALL_SQUARE_DIMENSION;
            v=y-(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    left_top.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    left_top.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x-(i+1)*SMALL_SQUARE_DIMENSION;
            v=y+(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    left_bottom.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    left_bottom.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x+(i+1)*SMALL_SQUARE_DIMENSION;
            v=y+(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    right_bottom.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    right_bottom.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x+(i+1)*SMALL_SQUARE_DIMENSION;
            v=y;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    right.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    right.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
            else
                break;
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x-(i+1)*SMALL_SQUARE_DIMENSION;
            v=y;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    left.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    left.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x;
            v=y+(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    bottom.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    bottom.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x;
            v=y-(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    top.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    top.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
    }
    bool check_square(Squares &a)
    {
        for(vector<Squares>::iterator i=right_top.begin();i!=right_top.end();i++)
        {
            if(*i==a)
                return true;
        }
        for(vector<Squares>::iterator i=left_top.begin();i!=left_top.end();i++)
        {
            if(*i==a)
                return true;
        }
        for(vector<Squares>::iterator i=right_bottom.begin();i!=right_bottom.end();i++)
        {
            if(*i==a)
                return true;
        }
        for(vector<Squares>::iterator i=left_bottom.begin();i!=left_bottom.end();i++)
        {
            if(*i==a)
                return true;
        }
        for(vector<Squares>::iterator i=right.begin();i!=right.end();i++)
        {
            if(*i==a)
                return true;
        }
        for(vector<Squares>::iterator i=left.begin();i!=left.end();i++)
        {
            if(*i==a)
                return true;
        }
        for(vector<Squares>::iterator i=bottom.begin();i!=bottom.end();i++)
        {
            if(*i==a)
                return true;
        }
        for(vector<Squares>::iterator i=top.begin();i!=top.end();i++)
        {
            if(*i==a)
                return true;
        }
        return false;
    }
};
int Queen::n=0;

class Knight:public Piece
{
    int move,i,j;
    string p_name;
    Squares Left_top_H,Right_top_H,Left_top_W,Right_top_W;
    Squares Left_bottom_W,Right_bottom_W,Left_bottom_H,Right_bottom_H;
  public:
    Knight(){}
    Knight(string p,const char *s,string s_name,int x,int y)
    :Piece(p,s,s_name)
    {
        if(!is_outer(x-SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION))
        {
            Left_top_H.set_square_values(x-SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION);
            find(Left_top_H);
            cout<<!is_friend(Left_top_H.return_piece_name())<<' '<<Left_top_H.return_piece_name()<<' ';
        }
        if(!is_outer(x-2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION))
        {
            Left_top_W.set_square_values(x-2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION);
            find(Left_top_W);
            cout<<!is_friend(Left_top_H.return_piece_name())<<' '<<Left_top_H.return_piece_name()<<endl;
        }
        if(!is_outer(x+SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION))
        {
            Right_top_H.set_square_values(x+SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION);
            find(Right_top_H);
            cout<<!is_friend(Right_top_H.return_piece_name())<<' '<<Right_top_H.return_piece_name()<<' ';
        }
        if(!is_outer(x+2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION))
        {
            Right_top_W.set_square_values(x+2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION);
            find(Right_top_W);
            cout<<!is_friend(Right_top_W.return_piece_name())<<' '<<Right_top_W.return_piece_name()<<endl;
        }
        if(!is_outer(x-SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION))
        {
            Left_bottom_H.set_square_values(x-SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION);
            find(Left_bottom_H);
            cout<<!is_friend(Left_bottom_H.return_piece_name())<<' '<<Left_bottom_H.return_piece_name()<<' ';
        }
        if(!is_outer(x-2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION))
        {
            Left_bottom_W.set_square_values(x-2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION);
            find(Left_bottom_W);
            cout<<!is_friend(Left_bottom_W.return_piece_name())<<' '<<Left_bottom_W.return_piece_name()<<endl;
        }
        if(!is_outer(x+SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION))
        {
            Right_bottom_H.set_square_values(x+SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION);
            find(Right_bottom_H);
            cout<<!is_friend(Right_bottom_H.return_piece_name())<<' '<<Right_bottom_H.return_piece_name()<<' ';
        }
        if(!is_outer(x+2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION))
        {
            Right_bottom_W.set_square_values(x+2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION);
            find(Right_bottom_W);
            cout<<!is_friend(Right_bottom_W.return_piece_name())<<' '<<Right_bottom_W.return_piece_name()<<endl;
        }
    }
    void print_render(int x,int y)
    {
        CP.X=x;
        CP.Y=y;
        create_texture(CP.X+18,CP.Y+16,75,90);
    }
    void set_piece_name(string s)
    {
        p_name=s;
    }
    void set_i_j(int x,int y)
    {
        cout<<"Called this function : "<<endl;
        i=x;j=y;
        cout<<" I : "<<i<<" J : "<<j<<endl;
    }
    void change(int x,int y)
    {
        if(!is_outer(x-SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION))
        {
            Left_top_H.set_square_values(x-SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION);
            find(Left_top_H);
            cout<<!is_friend(Left_top_H.return_piece_name())<<' '<<Left_top_H.return_piece_name()<<' ';
        }
        if(!is_outer(x-2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION))
        {
            Left_top_W.set_square_values(x-2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION);
            find(Left_top_W);
            cout<<!is_friend(Left_top_H.return_piece_name())<<' '<<Left_top_H.return_piece_name()<<endl;
        }
        if(!is_outer(x+SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION))
        {
            Right_top_H.set_square_values(x+SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION);
            find(Right_top_H);
            cout<<!is_friend(Right_top_H.return_piece_name())<<' '<<Right_top_H.return_piece_name()<<' ';
        }
        if(!is_outer(x+2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION))
        {
            Right_top_W.set_square_values(x+2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION);
            find(Right_top_W);
            cout<<!is_friend(Right_top_W.return_piece_name())<<' '<<Right_top_W.return_piece_name()<<endl;
        }
        if(!is_outer(x-SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION))
        {
            Left_bottom_H.set_square_values(x-SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION);
            find(Left_bottom_H);
            cout<<!is_friend(Left_bottom_H.return_piece_name())<<' '<<Left_bottom_H.return_piece_name()<<' ';
        }
        if(!is_outer(x-2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION))
        {
            Left_bottom_W.set_square_values(x-2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION);
            find(Left_bottom_W);
            cout<<!is_friend(Left_bottom_W.return_piece_name())<<' '<<Left_bottom_W.return_piece_name()<<endl;
        }
        if(!is_outer(x+SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION))
        {
            Right_bottom_H.set_square_values(x+SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION);
            find(Right_bottom_H);
            cout<<!is_friend(Right_bottom_H.return_piece_name())<<' '<<Right_bottom_H.return_piece_name()<<' ';
        }
        if(!is_outer(x+2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION))
        {
            Right_bottom_W.set_square_values(x+2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION);
            find(Right_bottom_W);
            cout<<!is_friend(Right_bottom_W.return_piece_name())<<' '<<Right_bottom_W.return_piece_name()<<endl;
        }
    }
    void display()
    {
        cout<<"U and V : "<<sq_i<<' '<<sq_j<<endl;
        cout<<"That square : "<<SQ[sq_i][sq_j]<<endl;
        cout<<"Piece name : "<<piece_name<<endl;
        cout<<"CP.X and CP.Y : "<<CP.X<<' '<<CP.Y<<' '<<CP.H<<' '<<CP.W<<endl;
    }
    bool is_friend(string a)
    {
        cout<<" I : "<<i<<" J : "<<j<<endl;
        cout<<"The Piecename : "<<SQ[i][j].return_piece_name()<<" The given string : "<<a<<endl;
        if(isupper(a[0]) && isupper(SQ[i][j].return_piece_name()[0]))
            return true;
        else if(islower(a[0]) && islower(SQ[i][j].return_piece_name()[0]))
            return true;
        else 
            return false;
    }
    void check_move(int x,int y)
    {
        //cout<<"Piece_name : "<<p_n<<endl;
        /*cout<<"knight validities"<<endl;
        cout<<!is_outer(x-SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION)<<' '<<!is_outer(x-2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION)<<endl;
        cout<<!is_outer(x+SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION)<<' '<<!is_outer(x+2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION)<<endl;
        cout<<!is_outer(x-SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION)<<' '<<!is_outer(x-2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION)<<endl;
        cout<<!is_outer(x+SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION)<<' '<<!is_outer(x+2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION)<<endl;
        */
        cout<<"The knight : "<<endl;
        display();
        if(!is_outer(x-SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION))
        {
            Left_top_H.set_square_values(x-SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION);
            find(Left_top_H);
            cout<<!is_friend(Left_top_H.return_piece_name())<<' '<<Left_top_H.return_piece_name()<<' ';
            if(!is_friend(Left_top_H.return_piece_name()) || Left_top_H.return_piece_name()=="0")
                show_aim(Left_top_H);
        }
        if(!is_outer(x-2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION))
        {
            Left_top_W.set_square_values(x-2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION);
            find(Left_top_W);
            cout<<!is_friend(Left_top_H.return_piece_name())<<' '<<Left_top_H.return_piece_name()<<endl;
            if(!is_friend(Left_top_W.return_piece_name()) || Left_top_W.return_piece_name()=="0")
                show_aim(Left_top_W);
        }
        if(!is_outer(x+SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION))
        {
            Right_top_H.set_square_values(x+SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION);
            find(Right_top_H);
            cout<<!is_friend(Right_top_H.return_piece_name())<<' '<<Right_top_H.return_piece_name()<<' ';
            if(!is_friend(Right_top_H.return_piece_name()) || Right_top_H.return_piece_name()=="0")
                show_aim(Right_top_H);
        }
        if(!is_outer(x+2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION))
        {
            Right_top_W.set_square_values(x+2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION);
            find(Right_top_W);
            cout<<!is_friend(Right_top_W.return_piece_name())<<' '<<Right_top_W.return_piece_name()<<endl;
            if(!is_friend(Right_top_W.return_piece_name()) || Right_top_W.return_piece_name()=="0")
                show_aim(Right_top_W);
        }
        if(!is_outer(x-SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION))
        {
            Left_bottom_H.set_square_values(x-SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION);
            find(Left_bottom_H);
            cout<<!is_friend(Left_bottom_H.return_piece_name())<<' '<<Left_bottom_H.return_piece_name()<<' ';
            if(!is_friend(Left_bottom_H.return_piece_name()) || Left_bottom_H.return_piece_name()=="0")
                show_aim(Left_bottom_H);
        }
        if(!is_outer(x-2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION))
        {
            Left_bottom_W.set_square_values(x-2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION);
            find(Left_bottom_W);
            cout<<!is_friend(Left_bottom_W.return_piece_name())<<' '<<Left_bottom_W.return_piece_name()<<endl;
            if(!is_friend(Left_bottom_W.return_piece_name()) || Left_bottom_W.return_piece_name()=="0")
                show_aim(Left_bottom_W);
        }
        if(!is_outer(x+SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION))
        {
            Right_bottom_H.set_square_values(x+SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION);
            find(Right_bottom_H);
            cout<<!is_friend(Right_bottom_H.return_piece_name())<<' '<<Right_bottom_H.return_piece_name()<<' ';
            if(!is_friend(Right_bottom_H.return_piece_name()) || Right_bottom_H.return_piece_name()=="0")
                show_aim(Right_bottom_H);
        }
        if(!is_outer(x+2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION))
        {
            Right_bottom_W.set_square_values(x+2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION);
            find(Right_bottom_W);
            cout<<!is_friend(Right_bottom_W.return_piece_name())<<' '<<Right_bottom_W.return_piece_name()<<endl;
            if(!is_friend(Right_bottom_W.return_piece_name()) || Right_bottom_W.return_piece_name()=="0")
                show_aim(Right_bottom_W);
        }
        /*cout<<"Knight's friends "<<endl;
        cout<<"Coordinates of Left top H : "<<Left_top_H.get_X()<<','<<Left_top_H.get_Y()<<endl;
        cout<<!is_friend(Left_top_H.return_piece_name())<<" "<<Left_top_H.return_piece_name()<<"  "<<!is_friend(Left_top_H.return_piece_name())<<" "<<Left_top_H.return_piece_name()<<endl;
        cout<<!is_friend(Right_top_H.return_piece_name())<<' '<<Right_top_H.return_piece_name()<<' '<<!is_friend(Right_top_W.return_piece_name())<<' '<<Right_top_W.return_piece_name()<<endl;
        cout<<!is_friend(Left_bottom_H.return_piece_name())<<' '<<Left_bottom_H.return_piece_name()<<' '<<!is_friend(Left_bottom_W.return_piece_name())<<' '<<Left_bottom_W.return_piece_name()<<endl;
        cout<<!is_friend(Right_bottom_H.return_piece_name())<<' '<<Right_bottom_H.return_piece_name()<<' '<<!is_friend(Right_bottom_W.return_piece_name())<<' '<<Right_bottom_W.return_piece_name()<<endl;
        */
        
    }
    bool check_square(Squares &a)
    {
        if(a==Left_top_H || a==Left_bottom_H ||a==Right_top_H || a==Right_bottom_H|| a==Left_top_W || a==Left_bottom_W || a==Right_bottom_W || a==Right_bottom_W)
            return true;
        else
            return false;
    }
    Squares get_Left_top_H()
    {
        return Left_top_H;
    }
    Squares get_Right_top_H()
    {
        return Right_top_H;
    }
    Squares get_Left_top_W()
    {
        return Left_top_W;
    }
    Squares get_Right_top_W()
    {
        return Right_top_W;
    }
    Squares get_Left_bottom_H()
    {
        return Left_bottom_H;
    }
    Squares get_Right_bottom_H()
    {
        return Right_bottom_H;
    }
    Squares get_Left_bottom_W()
    {
        return Left_bottom_W;
    }
    Squares get_Right_bottom_W()
    {
        return Right_bottom_W;
    }
    void set_null()
    {
        Piece::set_null();
        Left_top_H.set_Null();
        Left_bottom_H.set_Null();
        Left_top_W.set_Null();
        Left_bottom_W.set_Null();
        Right_top_H.set_Null();
        Right_bottom_H.set_Null();
        Right_top_W.set_Null();
        Right_bottom_W.set_Null();
    }
};
class Bishop:public Piece
{
    int move;
    vector<Squares> right_top,left_top;
    vector<Squares> left_bottom,right_bottom;
  public:
    Bishop(){}
    Bishop(string p,const char *s,string s_name,int x,int y)
    :Piece(p,s,s_name)
    {
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x+(i+1)*SMALL_SQUARE_DIMENSION;
            v=y-(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                cout<<!is_friend(s.return_piece_name())<<' '<<s.return_piece_name()<<' '<<piece_name<<endl;
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    right_top.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    right_top.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
            else
                break;
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x-(i+1)*SMALL_SQUARE_DIMENSION;
            v=y-(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                cout<<!is_friend(s.return_piece_name())<<' '<<s.return_piece_name()<<endl;
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    left_top.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    left_top.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x-(i+1)*SMALL_SQUARE_DIMENSION;
            v=y+(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                cout<<!is_friend(s.return_piece_name())<<' '<<s.return_piece_name()<<endl;
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    left_bottom.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    left_bottom.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x+(i+1)*SMALL_SQUARE_DIMENSION;
            v=y+(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                cout<<!is_friend(s.return_piece_name())<<' '<<s.return_piece_name()<<endl;
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    right_bottom.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    right_bottom.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
    
    }
    void print_render(int x,int y)
    {
        CP.X=x;
        CP.Y=y;
        create_texture(CP.X+12,CP.Y+16,90,90);
    }   
    void set_piece_name(string s)
    {
        piece_name=s;
    }
    void change(int x,int y)
    {
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x+(i+1)*SMALL_SQUARE_DIMENSION;
            v=y-(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                cout<<!is_friend(s.return_piece_name())<<' '<<s.return_piece_name()<<' '<<piece_name<<endl;
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    right_top.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    right_top.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
            else
                break;
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x-(i+1)*SMALL_SQUARE_DIMENSION;
            v=y-(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                cout<<!is_friend(s.return_piece_name())<<' '<<s.return_piece_name()<<endl;
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    left_top.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    left_top.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x-(i+1)*SMALL_SQUARE_DIMENSION;
            v=y+(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                cout<<!is_friend(s.return_piece_name())<<' '<<s.return_piece_name()<<endl;
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    left_bottom.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    left_bottom.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x+(i+1)*SMALL_SQUARE_DIMENSION;
            v=y+(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                cout<<!is_friend(s.return_piece_name())<<' '<<s.return_piece_name()<<endl;
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    right_bottom.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    right_bottom.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
    }    
    void display()
    {
        cout<<"U and V : "<<sq_i<<' '<<sq_j<<endl;
        cout<<"That square : "<<SQ[sq_i][sq_j]<<endl;
        cout<<"Piece name : "<<piece_name<<endl;
        cout<<"CP.X and CP.Y : "<<CP.X<<' '<<CP.Y<<' '<<CP.H<<' '<<CP.W<<endl;
        cout<<"The left diagonal : "<<endl;
        for(vector<Squares>::iterator i=right_top.begin();i!=right_top.end();i++)
        {
            cout<<*i<<endl;
        }
        for(vector<Squares>::iterator i=left_top.begin();i!=left_top.end();i++)
        {
            cout<<*i<<endl;
        }
        for(vector<Squares>::iterator i=right_bottom.begin();i!=right_bottom.end();i++)
        {
            cout<<*i<<endl;
        }
        for(vector<Squares>::iterator i=left_bottom.begin();i!=left_bottom.end();i++)
        {
            cout<<*i<<endl;
        }
    }
    bool is_friend(string a)
    {
        if(isupper(a[0]) && isupper(piece_name[0]))
            return true;
        else if(islower(a[0]) && islower(piece_name[0]))
            return true;
        else 
            return false;
    }
    bool outer(int x,int y)
    {
        if(x<440 || y<30 || x>1224 || y>814)
            return true;
        else
            return false;
    }
    /*bool is_check(King &a)
    {
        for(vector<Squares>::iterator i=right_top.begin();i!=right_top.end();i++)
        {
            if(*i==SQ[a.get_sq_i()][a.get_sq_j()])
                return true;
        }
        for(vector<Squares>::iterator i=left_top.begin();i!=left_top.end();i++)
        {
            if(*i==SQ[a.get_sq_i()][a.get_sq_j()])
                return true;
        }
        for(vector<Squares>::iterator i=right_bottom.begin();i!=right_bottom.end();i++)
        {
            if(*i==SQ[a.get_sq_i()][a.get_sq_j()])
                return true;
        }
        for(vector<Squares>::iterator i=left_bottom.begin();i!=left_bottom.end();i++)
        {
            if(*i==SQ[a.get_sq_i()][a.get_sq_j()])
                return true;
        }
        return false;
    }*/
    void check_move(int x,int y)
    {
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x+(i+1)*SMALL_SQUARE_DIMENSION;
            v=y-(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                cout<<!is_friend(s.return_piece_name())<<' '<<s.return_piece_name()<<' '<<piece_name<<endl;
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    right_top.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    right_top.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
            else
                break;
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x-(i+1)*SMALL_SQUARE_DIMENSION;
            v=y-(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                cout<<!is_friend(s.return_piece_name())<<' '<<s.return_piece_name()<<endl;
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    left_top.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    left_top.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x-(i+1)*SMALL_SQUARE_DIMENSION;
            v=y+(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                cout<<!is_friend(s.return_piece_name())<<' '<<s.return_piece_name()<<endl;
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    left_bottom.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    left_bottom.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
        for(int i=0;i<8;i++)
        {
            int u,v;
            u=x+(i+1)*SMALL_SQUARE_DIMENSION;
            v=y+(i+1)*SMALL_SQUARE_DIMENSION;
            if(!outer(u,v))
            {
                Squares s;
                s.set_square_values(u,v);
                find(s);
                cout<<!is_friend(s.return_piece_name())<<' '<<s.return_piece_name()<<endl;
                if(!s.is_empty() && !is_friend(s.return_piece_name()))
                {
                    show_aim(s);
                    right_bottom.push_back(s);
                    break;
                }
                if(s.is_empty())
                {
                    show_aim(s);
                    right_bottom.push_back(s);
                    cout<<s<<endl;
                }
                else
                    break;
            }
        }
    }
    bool check_square(Squares &a)
    {
        for(vector<Squares>::iterator i=right_top.begin();i!=right_top.end();i++)
        {
            if(*i==a)
                return true;
        }
        for(vector<Squares>::iterator i=left_top.begin();i!=left_top.end();i++)
        {
            if(*i==a)
                return true;
        }
        for(vector<Squares>::iterator i=right_bottom.begin();i!=right_bottom.end();i++)
        {
            if(*i==a)
                return true;
        }
        for(vector<Squares>::iterator i=left_bottom.begin();i!=left_bottom.end();i++)
        {
            if(*i==a)
                return true;
        }
        return false;
    }
};


//Importing Textures for Black Pieces
Pawn b_pawn[8];
Rook b_rook[2];
Bishop b_bishop[2];
Knight b_knight[2];
Queen temp_queen("q1",black_queen,"d8",776,30);
vector<Queen> b_queen;


//Importing Textures for White Pieces
Pawn w_pawn[8];
Rook w_rook[2];
Knight w_knight[2];
Bishop w_bishop[2];
Queen temp_Queen("Q1",white_queen,"e1",776,814);
vector<Queen> w_queen;

class King:public Piece
{
    int move;
  public:
    Squares Left,Right,Top,Bottom;
    Squares Left_top,Left_bottom,Right_top,Right_bottom;
    King()
    {
        move=0;
    }
    King(string p,const char *s,string s_name)
    :Piece(p,s,s_name)
    {}
    void display()
    {
        cout<<"U and V : "<<sq_i<<' '<<sq_j<<endl;
        cout<<"That square : "<<SQ[get_sq_i()][get_sq_j()]<<endl;
        cout<<"Piece name : "<<piece_name<<endl;
        cout<<"CP.X and CP.Y : "<<CP.X<<' '<<CP.Y<<' '<<CP.H<<' '<<CP.W<<endl;
    }
    void print_render(int x,int y)
    {
        CP.X=x;
        CP.Y=y;
        create_texture(CP.X+10,CP.Y+11,95,95);
    }
    bool is_valid(Squares &a)
    {
        for(int i=0;i<8;i++)
        {
            for(int j=0;j<8;j++)
            {
                string s=SQ[i][j].return_piece_name();
                if(islower(piece_name[0]))
                {
                    if(s[0]=='P')
                    {
                        for(int k=0;k<8;k++)
                        {
                            if(w_pawn[k].check_square(a))
                                return false;
                        }
                    }
                    if(s[0]=='B')
                    {
                        for(int k=0;k<2;k++)
                        {
                            if(w_bishop[k].check_square(a))
                                return false;
                        }
                        move=1;                 
                    }
                    if(s[0]=='R')
                    {
                        for(int k=0;k<2;k++)
                        {
                            if(w_rook[k].check_square(a))
                                return false;
                        }
                        move=1;
                    }
                    if(s[0]=='N')
                    {
                        for(int k=0;k<2;k++)
                        {
                            if(w_knight[k].check_square(a))
                                return false;
                        }
                        move=1;
                    }
                    if(s[0]=='Q')
                    {
                        for(int k=0;k<w_queen.size();k++)
                        {
                            if(w_queen[k].check_square(a))
                                return false;
                        }
                        move=1;
                    }
                }
                else if(isupper(piece_name[0]))
                {
                    if(s[0]=='b')
                    {
                        for(int k=0;k<2;k++)
                        {
                            if(b_bishop[k].check_square(a))
                                return false;
                        }
                    }
                    if(s[0]=='r')
                    {
                        for(int k=0;k<2;k++)
                        {
                            if(b_rook[k].check_square(a))
                                return false;
                        }
                    }
                    if(s[0]=='n')
                    {
                        for(int k=0;k<2;k++)
                        {
                            if(b_knight[k].check_square(a))
                                return false;
                        }
                    }
                    if(s[0]=='Q')
                    {
                        for(int k=0;k<w_queen.size();k++)
                        {
                            if(w_queen[k].check_square(a))
                                return false;
                        }
                        move=1;
                    }
                }
            }
        }
        return true;
    }
    bool is_friend(string a)
    {
        if(isupper(a[0]) && isupper(piece_name[0]))
            return true;
        else if(islower(a[0]) && islower(piece_name[0]))
            return true;
        else 
            return false;
    }
    bool outer(int x,int y)
    {
        if(x<440 || y<30 || x>1224 || y>814)
            return true;
        else
            return false;
    }
    void check_move(int x,int y)
    {
        if(!outer(x-SMALL_SQUARE_DIMENSION,y))
        {
            Left.set_square_values(x-SMALL_SQUARE_DIMENSION,y);
            find(Left);
            if(!is_friend(Left.return_piece_name()) && is_valid(Left))
                show_aim(Left);
        }
        if(!outer(x+SMALL_SQUARE_DIMENSION,y))
        {
            Right.set_square_values(x+SMALL_SQUARE_DIMENSION,y);
            find(Right);
            if(!is_friend(Right.return_piece_name()) && is_valid(Right))
                show_aim(Right);
        }
        if(!outer(x,y-SMALL_SQUARE_DIMENSION))
        {
            Top.set_square_values(x,y-SMALL_SQUARE_DIMENSION);
            find(Top);
            if(!is_friend(Top.return_piece_name()) && is_valid(Top))
                show_aim(Top);
        }
        if(!outer(x,y+SMALL_SQUARE_DIMENSION))
        {
            Bottom.set_square_values(x,y+SMALL_SQUARE_DIMENSION);
            find(Bottom);
            if(!is_friend(Bottom.return_piece_name()) && is_valid(Bottom))
                show_aim(Bottom);
        }
        if(!outer(x-SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION))
        {
            Left_top.set_square_values(x-SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION);
            find(Left_top);
            if(!is_friend(Left_top.return_piece_name()) && is_valid(Left_top))
                show_aim(Left_top);
        }
        if(!outer(x+SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION))
        {
            Right_top.set_square_values(x+SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION);
            find(Right_top);
            if(!is_friend(Right_top.return_piece_name()) && is_valid(Right_top))
                show_aim(Right_top);
        }
        if(!outer(x-SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION))
        {
            Left_bottom.set_square_values(x-SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION);
            find(Left_bottom);
            if(!is_friend(Left_bottom.return_piece_name()) && is_valid(Left_bottom))
                show_aim(Left_bottom);
        }
        if(!outer(x+SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION))
        {
            Right_bottom.set_square_values(x+SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION);
            find(Right_bottom);
            if(!is_friend(Right_bottom.return_piece_name()) && is_valid(Right_bottom))
                show_aim(Right_bottom);
        }   
    }
    bool check_square(Squares &a)
    {
            if(Left==a && is_valid(Left)|| Right==a && is_valid(Right)|| Top==a && is_valid(Top)||Bottom==a && is_valid(Bottom))
                return true;
            else if(Left_top==a && is_valid(Left_top)||Right_top==a && is_valid(Right_top)||Left_bottom==a && is_valid(Left_bottom)||Right_bottom==a && is_valid(Right_bottom))
                return true;
            else
                return false;
    }
    
};

King b_king("k",black_king,"e8");

King w_king("K",white_king,"e1");
// A temporary function to create rectangles and texture it

void create_texture(SDL_Renderer *render,SDL_Texture *texture,int x,int y,int w,int h)
{
    SDL_Rect source; 
	source.x = 0;
    source.y =0;
  	source.w;
	source.h;
    SDL_QueryTexture(texture, NULL, NULL, &source.w, &source.h);
    SDL_Rect destination;
    destination.x = x;
    destination.y = y;
    destination.w = w;
    destination.h = h;
    SDL_RenderCopy(render,texture,&source,&destination);  
    SDL_RenderPresent(render);
}

void queen_print_render(Queen &a,int x,int y)
{
        a.CP.X=x;
        a.CP.Y=y;
        a.create_texture(a.CP.X+17,a.CP.Y+16,95,90);
}
//Initializing the board

void Board()
{
    SDL_Texture *Dark=IMG_LoadTexture(render.get_renderer(),"res/img/Board Black.png");
    SDL_Texture *Light=IMG_LoadTexture(render.get_renderer(),"res/img/Board White.png");
    if(Dark==NULL && Light==NULL)
    {
        cout<<"Failed to load Board image : "<<IMG_GetError()<<endl;
        return;
    }
    else
    {
        create_texture(render.get_renderer(),Light,440,30,896,896);
        create_texture(render.get_renderer(),Dark,440,30,896,896);
    }
}

// Setting up initial positions of Black Pieces

void initialize_Black()
{
    for(int i=0;i<8;i++)
    {
        string temp,t;
        stringstream s,stl;
        s<<7;stl<<1+i;
        s>>temp;stl>>t;
        b_pawn[i].piece_value("p"+t,black_pawn,alpha[i]+temp);
    }
    cout<<endl;
    b_queen.push_back(temp_queen);
    string temp,temp1,temp2;
    stringstream s,ss,sss;
    s<<8;ss<<1;sss<<2;
    s>>temp;ss>>temp1;sss>>temp2;
    b_bishop[0].piece_value("b1",black_bishop,alpha[2]+temp);//b_bishop[0].display();
    b_bishop[1].piece_value("b2",black_bishop,alpha[5]+temp);//b_bishop[1].display();
    b_knight[0].piece_value("n1",black_knight,alpha[1]+temp);b_knight[0].display();cout<<b_knight[0]<<endl;
    b_knight[1].piece_value("n2",black_knight,alpha[6]+temp);b_knight[1].display();
    b_rook[0].piece_value("r1",black_rook,alpha[0]+temp);//b_rook[0].display();
    b_rook[1].piece_value("r2",black_rook,alpha[7]+temp);//b_rook[1].display();cout<<endl;

}

// Setting up initial positions of Black Pieces

void initialize_White()
{
    for(int i=0;i<8;i++)
    {
        string temp,t;
        stringstream s,stl;
        s<<2;stl<<i+1;
        s>>temp;stl>>t;
        w_pawn[i].piece_value("P"+t,white_pawn,alpha[i]+temp);
    }
    cout<<endl;
    w_queen.push_back(temp_Queen);
    string temp1,temp2;
    stringstream s,ss;
    s<<1;ss<<2;
    s>>temp1;ss>>temp2;
    w_bishop[0].piece_value("B1",white_bishop,alpha[2]+temp1);w_bishop[0].change(440+SMALL_SQUARE_DIMENSION*2,30+SMALL_SQUARE_DIMENSION*7);w_bishop[0].display();
    w_bishop[1].piece_value("B2",white_bishop,alpha[5]+temp1);w_bishop[1].change(440+SMALL_SQUARE_DIMENSION*5,30+SMALL_SQUARE_DIMENSION*7);
    w_knight[0].piece_value("N1",white_knight,alpha[1]+temp1);w_knight[0].set_i_j(7,1);w_knight[0].display();
    w_knight[1].piece_value("N2",white_knight,alpha[6]+temp1);w_knight[1].set_i_j(7,6);w_knight[1].display();
    w_rook[0].piece_value("R1",white_rook,alpha[0]+temp1);//w_rook[0].display();
    w_rook[1].piece_value("R2",white_rook,alpha[7]+temp1);//w_rook[1].display();cout<<endl;

    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            int n;
            string s=SQ[i][j].return_piece_name();
            if(s=="0")
                continue;
            if(SQ[i][j].return_piece_name()[0]=='P')
                w_pawn[j].set_sq_i_and_j(i,j);
            else if(SQ[i][j].return_piece_name()[0]=='p')
                b_pawn[j].set_sq_i_and_j(i,j);
            else if(SQ[i][j].return_piece_name()[0]=='K')
                w_king.set_sq_i_and_j(i,j);
            else if(SQ[i][j].return_piece_name()[0]=='k')
                b_king.set_sq_i_and_j(i,j);
            /*else if(SQ[i][j].return_piece_name()[0]=='q')
            {
                stringstream ss;
                ss<<s[1];
                ss>>n;
                cout<<"Black"<<endl;
                for(int k=0;k<b_queen.size();k++)
                {
                    cout<<" N : "<<n<<" I : "<<k<<endl;
                    if(n==k)
                    {
                        b_queen[k].set_sq_i_and_j(i,j);
                        break;
                    }
                }
            }
            else if(SQ[i][j].return_piece_name()[0]=='Q')
            {
                stringstream ss;
                ss<<s[1];
                ss>>n;
                cout<<"White : "<<endl;
                for(int k=0;k<w_queen.size();k++)
                {
                    cout<<" N : "<<n<<" I : "<<k<<endl;
                    if(n==k)
                    {
                        w_queen[k].set_sq_i_and_j(i,j);
                        break;
                    }
                }
            }*/
            else
            {
                stringstream ss;
                ss<<s[1];
                ss>>n;
                //cout<<"White : "<<endl;
                for(int k=0;k<w_queen.size();k++)
                {
                    if(k==n-1 && SQ[i][j].return_piece_name()[0]=='Q')
                    {
                        //cout<<" N : "<<n<<" I : "<<k<<endl;
                        w_queen[k].set_sq_i_and_j(i,j);
                        // w_queen[k].display();
                        break;
                    }
                }
                for(int k=0;k<b_queen.size();k++)
                {
                    if(k==n-1 && SQ[i][j].return_piece_name()[0]=='q')
                    {
                        //cout<<" N : "<<n<<" I : "<<k<<endl;
                        b_queen[k].set_sq_i_and_j(i,j);
                        //b_queen[k].display();
                        break;
                    }
                }
                for(int k=0;k<2;k++)
                {
                    cout<<SQ[i][j].return_piece_name()<<' '<<k+1<<' '<<n<<endl;
                    if(SQ[i][j].return_piece_name()[0]=='b' && k==n-1)
                    {
                        //cout<<"This : "<<SQ[i][j]<<"The name : "<<SQ[i][j].return_piece_name()<<" K : "<<'('<<i<<','<<j<<')'<<endl;
                        b_bishop[k].set_sq_i_and_j(i,j);
                        // b_bishop[k].display();
                        continue;
                    }
                    else if(SQ[i][j].return_piece_name()[0]=='B' && k==n-1)
                    {
                        //cout<<"This : "<<SQ[i][j]<<"The name : "<<SQ[i][j].return_piece_name()<<" K : "<<'('<<i<<','<<j<<')'<<endl;
                        w_bishop[k].set_sq_i_and_j(i,j);
                        //w_bishop[k].display();
                        continue;
                    }
                    else if(SQ[i][j].return_piece_name()[0]=='n' && k==n-1)
                    {
                        //cout<<"Black knight : "<<endl;
                        //cout<<"This : "<<SQ[i][j]<<"The name : "<<b_knight[k].get_piece_name()<<" K : "<<'('<<i<<','<<j<<')'<<endl;
                        b_knight[k].set_sq_i_and_j(i,j);
                        continue;
                    }
                    else if(SQ[i][j].return_piece_name()[0]=='N' && k==n-1)
                    {
                        //cout<<"White knight : "<<endl;
                        //cout<<"This : "<<SQ[i][j]<<"The name : "<<w_knight[k].get_piece_name()<<" K : "<<'('<<i<<','<<j<<')'<<endl;
                        w_knight[k].set_sq_i_and_j(i,j);
                        //w_knight[k].display();
                        continue;
                    }
                    else if(SQ[i][j].return_piece_name()[0]=='r' && k==n-1)
                    {
                        //cout<<"This : "<<SQ[i][j]<<"The name : "<<SQ[i][j].return_piece_name()<<" K : "<<'('<<i<<','<<j<<')'<<endl;
                        b_rook[k].set_sq_i_and_j(i,j);
                        //b_rook[k].display();
                        continue;
                    }
                    else if(SQ[i][j].return_piece_name()[0]=='R' && k==n-1)
                    {
                        //cout<<"This : "<<SQ[i][j]<<"The name : "<<SQ[i][j].return_piece_name()<<" K : "<<'('<<i<<','<<j<<')'<<endl;
                        w_rook[k].set_sq_i_and_j(i,j);
                        //w_rook[k].display();
                        continue;
                    }
                }
            }
        }
    }

}

void draw_board()
{
    Board();
    cout<<endl;
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            string temp=SQ[i][j].return_piece_name();
            
            int x=SQ[i][j].get_X(),y=SQ[i][j].get_Y();
            
            if(temp=="0")
                continue;
            if('Q'==temp[0])
            {
                cout<<temp<<'\t';
                cout<<" Square : ("<<x<<','<<y<<')'<<endl; 
                w_queen[0].print_render(x,y);
                //cout<<"Yes inside"<<endl;
                continue;
            }
                
            if('q'==temp[0])
            {
                int in;
                stringstream ss;
                ss<<temp[1];
                ss>>in;
                cout<<temp<<'\t';
                cout<<" Square : ("<<x<<','<<y<<')'<<endl; 
                b_queen[0].print_render(x,y);
                //cout<<"Yes inside"<<endl;
                continue;
            }
                
            if('B'==temp[0])
            {
                int in;
                stringstream ss;
                ss<<temp[1];
                ss>>in;
                //cout<<"Piece Name : "<<w_bishop[in-1]<<endl;
                w_bishop[in-1].print_render(x,y);
            }
            if('b'==temp[0])
            {
                int in;
                stringstream ss;
                ss<<temp[1];
                ss>>in;
                // cout<<"Piece Name : "<<b_bishop[in-1]<<endl;
                b_bishop[in-1].print_render(x,y);
            }
                
            if('N'==temp[0])
            {
                int in;
                stringstream ss;
                ss<<temp[1];
                ss>>in;
                // cout<<"Piece Name : "<<w_knight[in-1]<<endl;
                w_knight[in-1].print_render(x,y);
            }
                
            if('n'==temp[0])
            {
                int in;
                stringstream ss;
                ss<<temp[1];
                ss>>in;
                //cout<<"Piece Name : "<<b_knight[in-1]<<endl;
                b_knight[in-1].print_render(x,y);
            }
            if('R'==temp[0])
            {
                int in;
                stringstream ss;
                ss<<temp[1];
                ss>>in;
                //cout<<"Piece Name : "<<w_rook[in-1]<<endl;
                w_rook[in-1].print_render(x,y);
            }
                
            if('r'==temp[0])
            {
                int in;
                stringstream ss;
                ss<<temp[1];
                ss>>in;
                //cout<<"Piece Name : "<<b_rook[in-1]<<endl;
                b_rook[in-1].print_render(x,y);  
                
            }
                
            if('P'==temp[0])
                w_pawn[0].print_render(x,y);              
            if('p'==temp[0])
                b_pawn[0].print_render(x,y);            
            else if("K"==temp)
                w_king.print_render(x,y);
            else if("k"==temp)
                b_king.print_render(x,y);
                      
        }
    }
}
/*
template <class T>
void destroy_pieces(T temp,char upper,char lower,string opponent)
{
    int n;
    if(temp.get_piece_name()[0]==upper)
    {
        switch(opponent[0])
        {
            case 'p':
            {
                stringstream ss;
                ss<<opponent[1];
                ss>>n;
                b_pawn[n-1].set_null();
                break;
            }
            case 'n':
            {
                stringstream ss;
                ss<<opponent[1];
                ss>>n;
                b_knight[n-1].set_null();
                break;
            }
            case 'b':
            {
                stringstream ss;
                ss<<opponent[1];
                ss>>n;
                b_bishop[n-1].set_null();
                break;
            }
            case 'r':
            {
                stringstream ss;
                ss<<opponent[1];
                ss>>n;
                b_rook[n-1].set_null();
                break;
            }
            case 'q':
            {
                stringstream ss;
                ss<<opponent[1];
                ss>>n;
                b_queen[n-1].set_null();
                break;
            }
        }
    }
    else if(temp.get_piece_name()[0]==lower)
    {
        switch(opponent[0])
        {
            case 'P':
            {
                stringstream ss;
                ss<<opponent[1];
                ss>>n;
                w_pawn[n-1].set_null();
                break;
            }
            case 'N':
            {
                stringstream ss;
                ss<<opponent[1];
                ss>>n;
                w_knight[n-1].set_null();
                break;
            }
            case 'B':
            {
                stringstream ss;
                ss<<opponent[1];
                ss>>n;
                w_bishop[n-1].set_null();
                break;
            }
            case 'R':
            {
                stringstream ss;
                ss<<opponent[1];
                ss>>n;
                w_rook[n-1].set_null();
                break;
            }
            case 'Q':
            {
                stringstream ss;
                ss<<opponent[1];
                ss>>n;
                w_queen[n-1].set_null();
                break;
            }
        }
    }
                            
}*/
//Main function

int main(int argc,char *argv[])
{
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Chunk *start_game_audio=Mix_LoadWAV(start_game_name);
    Mix_Chunk *end_game_audio=Mix_LoadWAV(end_game_name);
    Mix_Chunk *move_audio=Mix_LoadWAV(move_name);
    Mix_Chunk *capture_audio=Mix_LoadWAV(capture_name);

    if(start_game_audio==NULL)
        cout<<" Error in loading audio "<<start_game_name<<" : "<<Mix_GetError()<<endl;
    if(end_game_audio==NULL)
        cout<<" Error in loading audio "<<end_game_name<<" : "<<Mix_GetError()<<endl;
    if(move_audio==NULL)
        cout<<" Error in loading audio "<<move_name<<" : "<<Mix_GetError()<<endl;

    aim=IMG_LoadTexture(render.get_renderer(),"res/img/Aim.png");
    target=IMG_LoadTexture(render.get_renderer(),"res/img/Target.png");
    SET_SQUARES();
    Mix_PlayChannel(-1,start_game_audio,0);
    initialize_Black();
    initialize_White();
    draw_board();
    
    cout<<"Queen : "<<w_queen[0]<<endl;
    cout<<"Queen : "<<b_queen[0]<<endl;
    SDL_Event window_Event;
    bool isfirstclick=true;
    Squares temp;
    string temp_piece;
    Pawn s1;
    int sn1,sn2,sn3,sn4,sn6;
    Bishop s2;
    Rook s3;
    Knight s4;
    King s5;
    Queen s6;

    b_knight[0].set_piece_name("n1");
    b_knight[1].set_piece_name("n2");
    w_knight[0].set_piece_name("N1");
    w_knight[1].set_piece_name("N2");

    b_bishop[0].set_piece_name("b1");
    b_bishop[1].set_piece_name("b2");
    w_bishop[0].set_piece_name("B1");
    w_bishop[1].set_piece_name("B2");

    while(true)
    {
        if(SDL_PollEvent(&window_Event))
        {
            if(SDL_QUIT == window_Event.type)
            {
                Mix_PlayChannel(-1,end_game_audio,0);
                break;
            }
            if(SDL_MOUSEBUTTONDOWN == window_Event.type)
            {
                if(SDL_BUTTON_LEFT == window_Event.button.button)
                {
                    bool sound=100;
                    int x,y;
                    SDL_GetMouseState(&x,&y);
                    if(is_outer(x,y))
                        cout<<"Click is out of boundary"<<endl;
                    else if(isfirstclick)
                    {                        
                        int u,v;
                        for(int i=0;i<8;i++)
                        {
                            bool continue_=true;
                            for(int j=0;j<8;j++)
                            {
                                if(SQ[i][j].is_clicked(x,y))
                                {
                                    u=i;v=j;
                                    temp_piece=SQ[i][j].return_piece_name();
                                    temp=SQ[i][j];
                                    continue_=false;
                                    break;
                                }
                            }
                            if(continue_==false)
                                    break;
                        }
                        char ch=temp_piece[0];
                        switch(ch)
                        {
                            case 'p':{
                                char ch=temp_piece[1];
                                int num=(int)ch-48;
                                s1=b_pawn[num-1];
                                sn1=num-1;
                                isfirstclick=false;
                                s1.check_black(temp.get_X(),temp.get_Y());
                                break;
                            }
                            case 'P':{
                                char ch=temp_piece[1];
                                int num=(int)ch-48;
                                s1=w_pawn[num-1];
                                sn1=num-1;
                                isfirstclick=false;
                                //cout<<"The selected object : "<<s1<<" And that square : "<<temp<<endl;
                                s1.check_white(temp.get_X(),temp.get_Y());
                                break;
                            }
                            case 'b':{
                                char ch=temp_piece[1];
                                int num=(int)ch-48;
                                s2=b_bishop[num-1];
                                sn2=num-1;
                                isfirstclick=false;
                                cout<<"Black bishop clicked ! at "<<temp<<endl;
                                //cout<<"That condition : "<<(!isfirstclick && s2.is_not_Null() && temp_piece!="0")<<' '<<"s.is_not_Null() : "<<s2.is_not_Null()<<endl;
                                cout<<sn2<<' '<<s2<<endl;
                                s2.check_move(temp.get_X(),temp.get_Y());
                                break;
                            }
                            case 'B':
                            {
                                char ch=temp_piece[1];
                                int num=(int)ch-48;
                                s2=w_bishop[num-1];
                                sn2=num-1;
                                isfirstclick=false;
                                cout<<"White bishop clicked ! at "<<temp<<endl;
                                s2.check_move(temp.get_X(),temp.get_Y());
                                //cout<<"This is the condition for level : "<<(!isfirstclick && s2.is_not_Null() && temp_piece!="0")<<" s.is_not_Null() : "<<s2.is_not_Null()<<endl;
                                break;
                            }
                            case 'r':{
                                char ch=temp_piece[1];
                                int num=(int)ch-48;
                                s3=b_rook[num-1];
                                sn3=num-1;
                                isfirstclick=false;
                                cout<<"Black rook clicked ! at "<<temp<<endl;
                                //cout<<"That condition : "<<(!isfirstclick && s3.is_not_Null() && temp_piece!="0")<<' '<<"S.is_not_Null() : "<<s3.is_not_Null()<<endl;
                                cout<<sn3<<' '<<s3<<endl;
                                s3.check_move(temp.get_X(),temp.get_Y());
                                break;
                            }
                            case 'R':
                            {
                                char ch=temp_piece[1];
                                int num=(int)ch-48;
                                s3=w_rook[num-1];
                                sn3=num-1;
                                isfirstclick=false;
                                cout<<"White rook clicked ! at "<<temp<<endl;
                                s3.check_move(temp.get_X(),temp.get_Y());
                                
                                break;
                            }
                            case 'n':
                            {
                                char ch=temp_piece[0];
                                int num=(int)ch-48;
                                s4=b_knight[num-1];
                                sn4=num-1;
                                cout<<endl<<endl<<"this is the black knight before moving : "<<endl;
                                s4.display();
                                cout<<endl<<endl;
                                isfirstclick=false;
                                s4.check_move(temp.get_X(),temp.get_Y());
                                cout<<"This is the condition for level : "<<(s4.is_not_Null() && s4.check_square(SQ[u][v]))<<' '<<s4.is_not_Null()<<endl;
                                break;
                            }
                            case 'N':
                            {
                                char ch=temp_piece[0];
                                int num=(int)ch-48;
                                s4=w_knight[num-1];
                                sn4=num-1;
                                cout<<(temp_piece[1]=='1')<<' '<<temp_piece<<' '<<'1'<<endl;
                                if(temp_piece[1]=='1')
                                {
                                    w_knight[0].set_i_j(7,1);
                                    w_knight[0].set_piece_name("N1");
                                }
                                    
                                if(temp.return_piece_name()[1]=='2')
                                {
                                    w_knight[1].set_i_j(7,6);
                                    w_knight[1].set_piece_name("N2");
                                    w_knight[1].display();
                                }
                                    
                                cout<<endl<<endl<<"this is the white knight before moving : "<<endl;
                                cout<<temp_piece<<endl;
                                s4.display();
                                cout<<endl<<endl;
                                isfirstclick=false;
                                s4.check_move(temp.get_X(),temp.get_Y());
                                cout<<"This is the condition for level : "<<(s4.is_not_Null() && s4.check_square(SQ[u][v]))<<' '<<s4.is_not_Null()<<' '<<s4.check_square(SQ[u][v])<<endl;
                                break;
                            }
                            case 'q':{
                                char ch=temp_piece[1];
                                int num=(int)ch-48;
                                s6=b_queen[num-1];
                                sn6=num-1;
                                isfirstclick=false;
                                cout<<"Black queen clicked ! at "<<temp<<endl;
                                //cout<<"That condition : "<<(!isfirstclick && s3.is_not_Null() && temp_piece!="0")<<' '<<"S.is_not_Null() : "<<s3.is_not_Null()<<endl;
                                cout<<sn6<<' '<<s6<<endl;
                                s6.check_move(temp.get_X(),temp.get_Y());
                                break;
                            }
                            case 'Q':
                            {
                                char ch=temp_piece[1];
                                int num=(int)ch-48;
                                s6=w_queen[num-1];
                                sn6=num-1;
                                isfirstclick=false;
                                cout<<"White queen clicked ! at "<<temp<<endl;
                                s6.check_move(temp.get_X(),temp.get_Y());
                                cout<<"This is the condition for level : "<<(!isfirstclick && s6.is_not_Null() && temp_piece!="0")<<" s.is_not_Null() : "<<s6.is_not_Null()<<endl;
                                break;
                            }
                            case 'K':
                            {
                                char ch=temp_piece[1];
                                s5=w_king;
                                isfirstclick=false;
                                cout<<"White queen clicked ! at "<<temp<<endl;
                                s5.check_move(temp.get_X(),temp.get_Y());
                                //cout<<"This is the condition for level : "<<(!isfirstclick && s6.is_not_Null() && temp_piece!="0")<<" s.is_not_Null() : "<<s6.is_not_Null()<<endl;
                                break;
                            }
                            case 'k':
                            {
                                char ch=temp_piece[1];
                                s5=b_king;
                                isfirstclick=false;
                                cout<<"White queen clicked ! at "<<temp<<endl;
                                s5.check_move(temp.get_X(),temp.get_Y());
                                //cout<<"This is the condition for level : "<<(!isfirstclick && s6.is_not_Null() && temp_piece!="0")<<" s.is_not_Null() : "<<s6.is_not_Null()<<endl;
                                break;
                            }
                        }
                    }

                    else if(!isfirstclick)
                    {
                        
                        int u,v;
                        for(int i=0;i<8;i++)
                        {
                            bool continue_=true;
                            for(int j=0;j<8;j++)
                            {
                                if(SQ[i][j].is_clicked(x,y))
                                {
                                    temp_piece=SQ[i][j].return_piece_name();
                                    temp=SQ[i][j];
                                    u=i;v=j;
                                    continue_=false;
                                    break;
                                }
                            }
                            if(continue_==false)
                                    break;
                        }
                        cout<<"X and Y : "<<'('<<x<<','<<y<<')'<<endl;
                        cout<<"U and V : "<<'('<<u<<','<<v<<')'<<endl;
                        //cout<<" S2 : "<<s2.is_not_Null()<<endl;
                        //cout<<(s3.is_not_Null())<<'|'<<(s3.is_not_Null() && temp_piece!="0")<<endl;
                        //cout<<(s1.get_double_for()==SQ[u][v] || (s1.get_for()==SQ[u][v] && s1.is_for_valid(SQ[u][v])))<<'|'<<(s1.get_piece_name()[0]=='P')<<endl;
                        if(s1.is_not_Null())
                        {
                            if(s1.get_double_for()==SQ[u][v] || (s1.get_for()==SQ[u][v] && s1.is_for_valid(SQ[u][v])))
                            {
                                int i=s1.get_sq_i(),j=s1.get_sq_j();
                                SQ[i][j].set_piece_name("0");
                                if(s1.get_piece_name()[0]=='p')
                                {
                                    s1=b_pawn[sn1];
                                    b_pawn[sn1].set_sq_i_and_j(u,v);
                                }
                                else if(s1.get_piece_name()[0]=='P')
                                {
                                    s1=w_pawn[sn1];
                                    w_pawn[sn1].set_sq_i_and_j(u,v);
                                }
                                cout<<"Square name : "<<SQ[u][v].return_square_name()<<' '<<SQ[u][v].return_square_name()[1]<<' '<<(s1.get_piece_name()[0]=='p' && SQ[u][v].return_square_name()[1]=='1' )<<' '<<(s1.get_piece_name()[0]=='P' && SQ[u][v].return_square_name()[1]=='8' )<<endl;
                                if(SQ[u][v].return_square_name()[1]=='1' )
                                {
                                    cout<<"INSIDE THE CHANGE"<<endl;
                                    SQ[u][v].set_piece_name("q1");
                                }
                                else if(SQ[u][v].return_square_name()[1]=='8')
                                {
                                    cout<<"INSIDE THE CHANGE"<<endl;
                                     SQ[u][v].set_piece_name("Q1");
                                }
                                   
                                else
                                    SQ[u][v].set_piece_name(s1.get_piece_name());
                                sound=0;
                            }
                            else if((s1.get_left_diag()==SQ[u][v] && s1.is_left_valid(SQ[u][v])) || (s1.get_right_diag()==SQ[u][v] && s1.is_right_valid(SQ[u][v])))
                            {
                                int n;
                                int i=s1.get_sq_i(),j=s1.get_sq_j();
                                string opponent=SQ[i][j].return_piece_name();
                                SQ[i][j].set_piece_name("0");
                                if(s1.get_piece_name()[0]=='p')
                                {
                                    s1=b_pawn[sn1];
                                    b_pawn[sn1].set_sq_i_and_j(u,v);
                                }
                                else if(s1.get_piece_name()[0]=='P')
                                {
                                    s1=w_pawn[sn1];
                                    w_pawn[sn1].set_sq_i_and_j(u,v);
                                }
                                cout<<"Square name : "<<SQ[u][v].return_square_name()<<' '<<SQ[u][v].return_square_name()[1]<<' '<<(s1.get_piece_name()[0]=='p' && SQ[u][v].return_square_name()[1]=='1' )<<' '<<(s1.get_piece_name()[0]=='P' && SQ[u][v].return_square_name()[1]=='8' )<<' '<<char(Queen::inc()+1)<<endl;
                                if(SQ[u][v].return_square_name()[1]=='1' )
                                {
                                    cout<<"INSIDE THE CHANGE"<<endl;
                                    SQ[u][v].set_piece_name("q1");
                                }
                                else if(SQ[u][v].return_square_name()[1]=='8')
                                {
                                    cout<<"INSIDE THE CHANGE"<<endl;
                                     SQ[u][v].set_piece_name("Q1");
                                }
                                   
                                else
                                    SQ[u][v].set_piece_name(s1.get_piece_name());
                                SQ[u][v].set_piece_name(s1.get_piece_name());
                                //destroy_pieces(s1,'P','p',opponent);
                            }
                            cout<<"Moved to "<<SQ[s1.get_sq_i()][s1.get_sq_j()]<<endl;
                            for(int i=0;i<8;i++)
                            {
                                for(int j=0;j<8;j++)
                                    SQ[i][j].display();
                                cout<<endl;
                            }
                            draw_board();
                            isfirstclick=true;
                            if(sound==0)
                                Mix_PlayChannel(-1,move_audio,0);
                            s1.set_null();
                        }
                        
                        else if(s2.is_not_Null() && s2.check_square(SQ[u][v]))
                        {
                            int n;
                            cout<<"Inside"<<endl;
                            cout<<"Piece_name : "<<s2.get_piece_name()<<' '<<temp_piece<<endl;
                                int i=s2.get_sq_i(),j=s2.get_sq_j();
                                cout<<"The current : "<<SQ[i][j]<<"The clicked one : "<<SQ[u][v]<<endl;
                                string opponent=SQ[i][j].return_piece_name();
                                SQ[i][j].set_piece_name("0");
                                
                                /*cout<<(s2.get_piece_name()[0]=='b')<<'|'<<(s2.get_piece_name()[0]=='B')<<endl;*/
                                if(s2.get_piece_name()[0]=='b')
                                {
                                    s2=b_bishop[sn2];
                                    b_bishop[sn2].set_sq_i_and_j(u,v);
                                }
                                else if(s2.get_piece_name()[0]=='B')
                                {
                                    s2=w_bishop[sn2];
                                    cout<<w_bishop[sn2]<<endl;
                                    w_bishop[sn2].set_sq_i_and_j(u,v);
                                }
                                SQ[u][v].set_piece_name(s2.get_piece_name());
                                //destroy_pieces(s2,'B','b',opponent);
                                sound=0;
                                
                            cout<<"Moved to "<<SQ[s2.get_sq_i()][s2.get_sq_j()]<<endl;
                            for(int i=0;i<8;i++)
                            {
                                for(int j=0;j<8;j++)
                                    SQ[i][j].display();
                                cout<<endl;
                            }
                            draw_board();
                            isfirstclick=true;
                            if(sound==0)
                                Mix_PlayChannel(-1,move_audio,0);
                            s2.set_null();
                        }
                        else if(s3.is_not_Null() && s3.check_square(SQ[u][v]))
                        {
                            int n;
                            cout<<"Inside"<<endl;
                            cout<<"Piece_name : "<<s3.get_piece_name()<<' '<<temp_piece<<endl;
                                int i=s3.get_sq_i(),j=s3.get_sq_j();
                                cout<<"The current : "<<SQ[i][j]<<"The clicked one : "<<SQ[u][v]<<endl;
                                string opponent=SQ[i][j].return_piece_name();
                                SQ[i][j].set_piece_name("0");
                                
                                /*cout<<(s2.get_piece_name()[0]=='b')<<'|'<<(s2.get_piece_name()[0]=='B')<<endl;*/
                                if(s3.get_piece_name()[0]=='r')
                                {
                                    s3=b_rook[sn3];
                                    b_rook[sn3].set_sq_i_and_j(u,v);
                                }
                                else if(s3.get_piece_name()[0]=='R')
                                {
                                    s3=w_rook[sn3];
                                    cout<<w_rook[sn3]<<endl;
                                    w_rook[sn3].set_sq_i_and_j(u,v);
                                }
                                if(SQ[u][v].return_piece_name()!="0")
                                    sound=1;
                                else sound=0;
                                SQ[u][v].set_piece_name(s3.get_piece_name());
                                //destroy_pieces(s3,'R','r',opponent);
                                
                            cout<<"Moved to "<<SQ[s3.get_sq_i()][s3.get_sq_j()]<<endl;
                            for(int i=0;i<8;i++)
                            {
                                for(int j=0;j<8;j++)
                                    SQ[i][j].display();
                                cout<<endl;
                            }
                            draw_board();
                            isfirstclick=true;
                            if(sound==0)
                                Mix_PlayChannel(-1,move_audio,0);
                            s3.set_null();
                        }
                        else if(s4.is_not_Null() && s4.check_square(SQ[u][v]))
                        {
                            int n;
                            cout<<"Inside"<<endl;
                            cout<<"Piece_name : "<<s4.get_piece_name()<<' '<<temp_piece<<endl;
                                int i=s4.get_sq_i(),j=s4.get_sq_j();
                                string opponent=SQ[i][j].return_piece_name();
                                SQ[i][j].set_piece_name("0");
                                
                                /*cout<<(s2.get_piece_name()[0]=='b')<<'|'<<(s2.get_piece_name()[0]=='B')<<endl;*/
                                if(s4.get_piece_name()[0]=='n')
                                {
                                    s4=b_knight[sn4];
                                    b_knight[sn4].set_sq_i_and_j(u,v);
                                }
                                else if(s4.get_piece_name()[0]=='N')
                                {
                                    s4=w_knight[sn4];
                                    cout<<w_knight[sn4]<<endl;
                                    w_knight[sn4].set_sq_i_and_j(u,v);
                                }
                                SQ[u][v].set_piece_name(s4.get_piece_name());
                                //destroy_pieces(s4,'N','n',opponent);
                                sound=0;
                                
                            cout<<"Moved to "<<SQ[s4.get_sq_i()][s4.get_sq_j()]<<endl;
                            for(int i=0;i<8;i++)
                            {
                                for(int j=0;j<8;j++)
                                    SQ[i][j].display();
                                cout<<endl;
                            }
                            draw_board();
                            isfirstclick=true;
                            if(sound==0)
                                Mix_PlayChannel(-1,move_audio,0);
                            s4.set_null();
                        }
                        else if(s5.is_not_Null() && s5.check_square(SQ[u][v]))
                        {
                            int i=s5.get_sq_i(),j=s5.get_sq_j();
                            string opponent=SQ[i][j].return_piece_name();
                            SQ[i][j].set_piece_name("0");
                            cout<<SQ[i][j]<<endl;
                            if(s5.get_piece_name()[0]=='k')
                            {
                                s5=b_king;
                                b_king.set_sq_i_and_j(u,v);
                            }
                            else if(s5.get_piece_name()[0]=='K')
                            {
                                s5=w_king;
                                w_king.set_sq_i_and_j(u,v);
                            }
                            SQ[u][v].set_piece_name(s5.get_piece_name());
                            sound=0;
                            cout<<"Moved to "<<SQ[s5.get_sq_i()][s5.get_sq_j()]<<endl;
                            for(int i=0;i<8;i++)
                            {
                                for(int j=0;j<8;j++)
                                    SQ[i][j].display();
                                cout<<endl;
                            }
                            draw_board();
                            isfirstclick=true;
                            if(sound==0)
                                Mix_PlayChannel(-1,move_audio,0);
                            s5.set_null();
                        }
                        else if(s6.is_not_Null() && s6.check_square(SQ[u][v]))
                        {
                            int n;
                            cout<<"Inside"<<endl;
                            cout<<"Piece_name : "<<s6.get_piece_name()<<' '<<temp_piece<<endl;
                                int i=s6.get_sq_i(),j=s6.get_sq_j();
                                cout<<"The current : "<<SQ[i][j]<<"The clicked one : "<<SQ[u][v]<<endl;
                                string opponent=SQ[i][j].return_piece_name();
                                SQ[i][j].set_piece_name("0");
                                
                                /*cout<<(s2.get_piece_name()[0]=='b')<<'|'<<(s2.get_piece_name()[0]=='B')<<endl;*/
                                if(s6.get_piece_name()[0]=='q')
                                {
                                    s6=b_queen[sn6];
                                    b_queen[sn6].set_sq_i_and_j(u,v);
                                }
                                else if(s6.get_piece_name()[0]=='Q')
                                {
                                    s6=w_queen[sn6];
                                    cout<<w_queen[sn6]<<endl;
                                    w_queen[sn6].set_sq_i_and_j(u,v);
                                }
                                SQ[u][v].set_piece_name(s6.get_piece_name());
                                //destroy_pieces(s6,'Q','q',opponent);
                                sound=0;

                            cout<<"Moved to "<<SQ[s6.get_sq_i()][s6.get_sq_j()]<<endl;
                            for(int i=0;i<8;i++)
                            {
                                for(int j=0;j<8;j++)
                                    SQ[i][j].display();
                                cout<<endl;
                            }
                            draw_board();
                            isfirstclick=true;
                            if(sound==0)
                                Mix_PlayChannel(-1,move_audio,0);
                            s6.set_null();
                        }
                        else
                        {
                            for(int i=0;i<8;i++)
                            {
                                for(int j=0;j<8;j++)
                                    SQ[i][j].display();
                                cout<<endl;
                            }
                            draw_board();
                            isfirstclick=true;
                            if(sound==0)
                                Mix_PlayChannel(-1,move_audio,0);
                        }
                    }
                }
            }
        }
    }
    SDL_Quit();
    return EXIT_SUCCESS;
}
