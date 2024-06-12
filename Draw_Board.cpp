void draw_board()
{
    Board();
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            string temp=SQ[i][j].return_piece_name();
            int x=SQ[i][j].get_X(),y=SQ[i][j].get_Y();
            if(temp=="0")
                break;
            if(w_pawn[j].get_piece_name()==temp)
            {
                w_pawn[j].set_square(SQ[i][j]);
                w_pawn[j].print_render(x,y);
            }
            else if(b_pawn[j].get_piece_name()==temp)
            {
                b_pawn[j].set_square(SQ[i][j]);
                b_pawn[j].print_render(x,y);
            }
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
                        w_knight[k].print_render(x,y);
                    if(b_knight[k].get_piece_name()==temp)
                        b_knight[k].print_render(x,y);
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