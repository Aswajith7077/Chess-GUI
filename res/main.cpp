#include<iostream>
#include<sstream>
#include<string.h>
#include<vector>
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

string a[][8]={"r1","n1","b1","q","k","b2","n2","r2",
               "p1","p2","p3","p4","p5","p6","p7","p8",
               "0","0","0","0","0","0","0","0",
               "0","0","0","0","0","0","0","0",
               "0","0","0","0","0","0","0","0",
               "0","0","0","0","0","0","0","0",
               "P1","P2","P3","P4","P5","P6","P7","P8",
               "R1","N1","B1","Q","K","B2","N2","R2"};
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
        window = SDL_CreateWindow("HI",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WIDTH,HEIGHT,SDL_WINDOW_ALLOW_HIGHDPI);
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
            cout<<"Failed to load "<<file_name<<" : "<<IMG_GetError()<<endl;
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
    }
    
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
            initial_move++;
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
        if(!left_diag.is_empty() && !is_friend(a.return_piece_name()))
            return true;
        else
            return false; 
    }
    bool is_right_valid(Squares &a)
    {
        if(!right_diag.is_empty() && !is_friend(a.return_piece_name()))
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
  public:
    Rook(){}
    Rook(string p,const char *s,string s_name)
    :Piece(p,s,s_name)
    {}
    void print_render(int x,int y)
    {
        CP.X=x;
        CP.Y=y;
        create_texture(CP.X+17,CP.Y+16,75,90);
    }
};
class Queen:public Piece
{
    int move;
    vector<Squares> right_top,left_top;
    vector<Squares> left_bottom,right_bottom;
 public:
    Queen(){}
    Queen(string p,const char *s,string s_name)
    :Piece(p,s,s_name)
    {}
    void print_render(int x,int y)
    {
        CP.X=x;
        CP.Y=y;
        create_texture(CP.X+9,CP.Y+16,95,90);
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
            if(!is_outer(u,v))
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
            if(!is_outer(u,v))
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
            if(!is_outer(u,v))
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
            if(!is_outer(u,v))
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
    }
};
class King:public Piece
{
    int move;
  public:
    King(){}
    King(string p,const char *s,string s_name)
    :Piece(p,s,s_name)
    {}
    void print_render(int x,int y)
    {
        CP.X=x;
        CP.Y=y;
        create_texture(CP.X+10,CP.Y+11,95,95);
    }
};
class Knight:public Piece
{
    int move;
    Squares Left_top_H,Right_top_H,Left_top_W,Right_top_W;
    Squares Left_bottom_W,Right_bottom_W,Left_bottom_H,Right_bottom_H;
  public:
    Knight(){}
    Knight(string p,const char *s,string s_name)
    :Piece(p,s,s_name)
    {}
    void print_render(int x,int y)
    {
        CP.X=x;
        CP.Y=y;
        create_texture(CP.X+18,CP.Y+16,75,90);
    }
    void set_piece_name(string s)
    {
        piece_name=s;
    }
    bool is_friend(string a)
    {
        if(isupper(a[0]) && isupper(piece_name[0]))
            return false;
        else if(islower(a[0]) && islower(piece_name[0]))
            return false;
        else 
            return true;
    }
    void check_move(int x,int y)
    {
        cout<<"Piece_name : "<<piece_name<<endl;
        /*cout<<"knight validities"<<endl;
        cout<<!is_outer(x-SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION)<<' '<<!is_outer(x-2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION)<<endl;
        cout<<!is_outer(x+SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION)<<' '<<!is_outer(x+2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION)<<endl;
        cout<<!is_outer(x-SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION)<<' '<<!is_outer(x-2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION)<<endl;
        cout<<!is_outer(x+SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION)<<' '<<!is_outer(x+2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION)<<endl;
        */
        
        if(!is_outer(x-SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION))
        {
            Left_top_H.set_square_values(x-SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION);
            find(Left_top_H);
            cout<<!is_friend(Left_top_H.return_piece_name())<<' '<<Left_top_H.return_piece_name()<<' ';
            if(!is_friend(Left_top_H.return_piece_name()))
                show_aim(Left_top_H);
        }
        if(!is_outer(x-2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION))
        {
            Left_top_W.set_square_values(x-2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION);
            find(Left_top_W);
            cout<<!is_friend(Left_top_H.return_piece_name())<<' '<<Left_top_H.return_piece_name()<<endl;
            if(!is_friend(Left_top_H.return_piece_name()))
                show_aim(Left_top_W);
        }
        if(!is_outer(x+SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION))
        {
            Right_top_H.set_square_values(x+SMALL_SQUARE_DIMENSION,y-2*SMALL_SQUARE_DIMENSION);
            find(Right_top_H);
            cout<<!is_friend(Right_top_H.return_piece_name())<<' '<<Right_top_H.return_piece_name()<<' ';
            if(!is_friend(Right_top_H.return_piece_name()))
                show_aim(Right_top_H);
        }
        if(!is_outer(x+2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION))
        {
            Right_top_W.set_square_values(x+2*SMALL_SQUARE_DIMENSION,y-SMALL_SQUARE_DIMENSION);
            find(Right_top_W);
            cout<<!is_friend(Right_top_W.return_piece_name())<<' '<<Right_top_W.return_piece_name()<<endl;
            if(!is_friend(Right_top_W.return_piece_name()))
                show_aim(Right_top_W);
        }
        if(!is_outer(x-SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION))
        {
            Left_bottom_H.set_square_values(x-SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION);
            find(Left_bottom_H);
            cout<<!is_friend(Left_bottom_H.return_piece_name())<<' '<<Left_bottom_H.return_piece_name()<<' ';
            if(!is_friend(Left_bottom_H.return_piece_name()))
                show_aim(Left_bottom_H);
        }
        if(!is_outer(x-2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION))
        {
            Left_bottom_W.set_square_values(x-2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION);
            find(Left_bottom_W);
            cout<<!is_friend(Left_bottom_W.return_piece_name())<<' '<<Left_bottom_W.return_piece_name()<<endl;
            if(!is_friend(Left_bottom_W.return_piece_name()))
                show_aim(Left_bottom_W);
        }
        if(!is_outer(x+SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION))
        {
            Right_bottom_H.set_square_values(x+SMALL_SQUARE_DIMENSION,y+2*SMALL_SQUARE_DIMENSION);
            find(Right_bottom_H);
            cout<<!is_friend(Right_bottom_H.return_piece_name())<<' '<<Right_bottom_H.return_piece_name()<<' ';
            if(!is_friend(Right_bottom_H.return_piece_name()))
                show_aim(Right_bottom_H);
        }
        if(!is_outer(x+2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION))
        {
            Right_bottom_W.set_square_values(x+2*SMALL_SQUARE_DIMENSION,y+SMALL_SQUARE_DIMENSION);
            find(Right_bottom_W);
            cout<<!is_friend(Right_bottom_W.return_piece_name())<<' '<<Right_bottom_W.return_piece_name()<<endl;
            if(!is_friend(Right_bottom_W.return_piece_name()))
                show_aim(Right_bottom_W);
        }
        /*cout<<"Knight's friends "<<endl;
        cout<<"Coordinates of Left top H : "<<Left_top_H.get_X()<<','<<Left_top_H.get_Y()<<endl;
        cout<<!is_friend(Left_top_H.return_piece_name())<<' '<<Left_top_H.return_piece_name()<<' '<<!is_friend(Left_top_H.return_piece_name())<<' '<<Left_top_H.return_piece_name()<<endl;
        cout<<!is_friend(Right_top_H.return_piece_name())<<' '<<Right_top_H.return_piece_name()<<' '<<!is_friend(Right_top_W.return_piece_name())<<' '<<Right_top_W.return_piece_name()<<endl;
        cout<<!is_friend(Left_bottom_H.return_piece_name())<<' '<<Left_bottom_H.return_piece_name()<<' '<<!is_friend(Left_bottom_W.return_piece_name())<<' '<<Left_bottom_W.return_piece_name()<<endl;
        cout<<!is_friend(Right_bottom_H.return_piece_name())<<' '<<Right_bottom_H.return_piece_name()<<' '<<!is_friend(Right_bottom_W.return_piece_name())<<' '<<Right_bottom_W.return_piece_name()<<endl;
        */
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

};
class Bishop:public Piece
{
    int move;
    vector<Squares> right_top,left_top;
    vector<Squares> left_bottom,right_bottom;
  public:
    Bishop(){}
    Bishop(string p,const char *s,string s_name)
    :Piece(p,s,s_name)
    {}
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
    void display()
    {
        cout<<"Piece name : "<<piece_name<<endl;
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
            if(!is_outer(u,v))
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
            if(!is_outer(u,v))
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
            if(!is_outer(u,v))
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
            if(!is_outer(u,v))
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
            if(*i==a && !is_friend(a.return_piece_name()))
                return true;
        }
        for(vector<Squares>::iterator i=left_top.begin();i!=left_top.end();i++)
        {
            if(*i==a && !is_friend(a.return_piece_name()))
                return true;
        }
        for(vector<Squares>::iterator i=right_bottom.begin();i!=right_bottom.end();i++)
        {
            if(*i==a && !is_friend(a.return_piece_name()))
                return true;
        }
        for(vector<Squares>::iterator i=left_bottom.begin();i!=left_bottom.end();i++)
        {
            if(*i==a && !is_friend(a.return_piece_name()))
                return true;
        }
    }
};


//Importing Textures for Black Pieces
Pawn b_pawn[8];
Rook b_rook[2];
Bishop b_bishop[2];
Knight b_knight[2];
Queen b_queen("q",black_queen,"d8");
King b_king("k",black_king,"e8");

//Importing Textures for White Pieces
Pawn w_pawn[8];
Rook w_rook[2];
Knight w_knight[2];
Bishop w_bishop[2];
Queen w_queen("Q",white_queen,"e1");
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
        b_pawn[i].display();
    }
    cout<<endl;
    b_queen.display();
    b_king.display();
    string temp,temp1,temp2;
    stringstream s,ss,sss;
    s<<8;ss<<1;sss<<2;
    s>>temp;ss>>temp1;sss>>temp2;
    b_bishop[0].piece_value("b1",black_bishop,alpha[2]+temp);b_bishop[0].display();
    b_bishop[1].piece_value("b2",black_bishop,alpha[5]+temp);b_bishop[1].display();
    b_knight[0].piece_value("n1",black_knight,alpha[1]+temp);b_knight[0].display();cout<<b_knight[0]<<endl;
    b_knight[1].piece_value("n2",black_knight,alpha[6]+temp);b_knight[1].display();
    b_rook[0].piece_value("r1",black_rook,alpha[0]+temp);b_rook[0].display();
    b_rook[1].piece_value("r2",black_rook,alpha[7]+temp);b_rook[1].display();cout<<endl;
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
    w_queen.display();
    w_king.display();
    string temp1,temp2;
    stringstream s,ss;
    s<<1;ss<<2;
    s>>temp1;ss>>temp2;
    w_bishop[0].piece_value("B1",white_bishop,alpha[2]+temp1);w_bishop[0].display();
    w_bishop[1].piece_value("B2",white_bishop,alpha[5]+temp1);w_bishop[1].display();
    w_knight[0].piece_value("N1",white_knight,alpha[1]+temp1);w_knight[0].display();
    w_knight[1].piece_value("N2",white_knight,alpha[6]+temp1);w_knight[1].display();cout<<w_knight[0]<<endl;
    w_rook[0].piece_value("R1",white_rook,alpha[0]+temp1);w_rook[0].display();
    w_rook[1].piece_value("R2",white_rook,alpha[7]+temp1);w_rook[1].display();cout<<endl;

    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            if(SQ[i][j].return_piece_name()[0]=='P')
                w_pawn[j].set_sq_i_and_j(i,j);
            else if(SQ[i][j].return_piece_name()[0]=='p')
                b_pawn[j].set_sq_i_and_j(i,j);
            else if(SQ[i][j].return_piece_name()[0]=='K')
                w_king.set_sq_i_and_j(i,j);
            else if(SQ[i][j].return_piece_name()[0]=='k')
                b_king.set_sq_i_and_j(i,j);
            else if(SQ[i][j].return_piece_name()[0]=='q')
                b_queen.set_sq_i_and_j(i,j);
            else if(SQ[i][j].return_piece_name()[0]=='Q')
                w_queen.set_sq_i_and_j(i,j);
            else
            {
                for(int k=0;k<2;k++)
                {
                    if(SQ[i][j].return_piece_name()[0]=='b')
                        b_bishop[k].set_sq_i_and_j(i,j);
                    else if(SQ[i][j].return_piece_name()[0]=='B')
                        w_bishop[k].set_sq_i_and_j(i,j);
                    else if(SQ[i][j].return_piece_name()[0]=='n')
                        b_knight[k].set_sq_i_and_j(i,j);
                    else if(SQ[i][j].return_piece_name()[0]=='N')
                        w_knight[k].set_sq_i_and_j(i,j);
                    else if(SQ[i][j].return_piece_name()[0]=='r')
                        b_rook[k].set_sq_i_and_j(i,j);
                    else if(SQ[i][j].return_piece_name()[0]=='R')
                        w_rook[k].set_sq_i_and_j(i,j);
                }
            }
        }
    }

}

void draw_board()
{
    Board();
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            string temp=SQ[i][j].return_piece_name();
            int x=SQ[i][j].get_X(),y=SQ[i][j].get_Y();
            if('P'==temp[0])
                w_pawn[0].print_render(x,y);
            if('p'==temp[0])
                b_pawn[0].print_render(x,y);
            if(w_queen.get_piece_name()==temp)
                w_queen.print_render(x,y);
            else if(b_queen.get_piece_name()==temp)
                b_queen.print_render(x,y);
            else if(w_king.get_piece_name()==temp)
                w_king.print_render(x,y);
            else if(b_king.get_piece_name()==temp)
                b_king.print_render(x,y);
            else
            {
                for(int k=0;k<2;k++)
                {
                    if(w_knight[k].get_piece_name()==temp)
                    {
                        //cout<<w_knight[k]<<endl;
                        w_knight[k].print_render(x,y);
                    }
                    if(b_knight[k].get_piece_name()==temp)
                    {
                        //cout<<b_knight[k]<<endl;
                        b_knight[k].print_render(x,y);
                    }
                    if(w_bishop[k].get_piece_name()==temp)
                        w_bishop[k].print_render(x,y);
                    if(b_bishop[k].get_piece_name()==temp)
                        b_bishop[k].print_render(x,y);
                    if(w_rook[k].get_piece_name()==temp)
                        w_rook[k].print_render(x,y);
                    if(b_rook[k].get_piece_name()==temp)
                        b_rook[k].print_render(x,y);
                }
            }
        }
    }
}


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
    initialize_White();
    initialize_Black();
    draw_board();
    
    SDL_Event window_Event;
    bool isfirstclick=true;
    Squares temp;
    string temp_piece;
    Pawn s1;
    int sn1,sn2,sn3,sn4;
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
                                cout<<"The selected object : "<<s1<<" And that square : "<<temp<<endl;
                                s1.check_white(temp.get_X(),temp.get_Y());
                                break;
                            }
                            case 'n':
                            {
                                char ch=temp_piece[0];
                                int num=(int)ch-48;
                                s4=b_knight[num-1];
                                sn4=num-1;
                                //isfirstclick=false;
                                s4.check_move(temp.get_X(),temp.get_Y());
                                break;
                            }
                            case 'N':
                            {
                                char ch=temp_piece[0];
                                int num=(int)ch-48;
                                s4=w_knight[num-1];
                                sn4=num-1;
                                //isfirstclick=false;
                                s4.check_move(temp.get_X(),temp.get_Y());
                                break;
                            }
                            case 'b':{
                                char ch=temp_piece[1];
                                int num=(int)ch-48;
                                s2=b_bishop[num-1];
                                sn2=num-1;
                                isfirstclick=false;
                                cout<<"Black bishop clicked ! at "<<temp<<endl;
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
                                break;
                            }
                        }
                    }

                    else if(!isfirstclick && s1.is_not_Null() && temp_piece!="0")
                    {
                        int u,v;
                        //cout<<(s1.get_left_diag()==SQ[u][v] && s1.is_left_valid()) || (s1.get_right_diag()==SQ[u][v] && s1.is_right_valid());
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
                        if(s1.get_double_for()==SQ[u][v] || (s1.get_for()==SQ[u][v] && s1.is_for_valid(SQ[u][v])))
                        {
                            cout<<s1.get_sq_i()<<' '<<s1.get_sq_j()<<endl;
                            
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
                            SQ[u][v].set_piece_name(s1.get_piece_name());
                            if(s1.get_piece_name()[0]=='P')
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
                                    case 'q':b_queen.set_null();break;
                                }
                            }
                            else if(s1.get_piece_name()[0]=='p')
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
                                    case 'Q':w_queen.set_null();break;
                                }
                            }
                        }
                        else if(s2.check_square(SQ[u][v]))
                        {
                            int i=s2.get_sq_i(),j=s2.get_sq_j();
                            string s=SQ[u][v].return_piece_name();
                            cout<<"I and J piece name : "<<s<<endl;
                            SQ[i][j].set_piece_name("0");
                            
                            if(s2.get_piece_name()[0]=='b')
                            {
                                s2=b_bishop[sn2];
                                b_bishop[sn2].set_sq_i_and_j(u,v);
                            }
                            else if(s2.get_piece_name()[0]=='B')
                            {
                                s2=w_bishop[sn2];
                                w_bishop[sn2].set_sq_i_and_j(u,v);
                            }
                            
                            SQ[u][v].set_piece_name(s2.get_piece_name());

                        }
                        cout<<"Moved to "<<SQ[s1.get_sq_i()][s1.get_sq_j()];
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
    SDL_Quit();
    return EXIT_SUCCESS;
}
