
/* Program to print and play checker games.
 Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
 September 2021, with the intention that it be modified by students
 to add functionality, as required by the assignment specification.
 Student Authorship Declaration:
 (1) I certify that except for the code provided in the initial skeleton file,
 the program contained in this submission is completely my own individual
 work, except where explicitly noted by further comments that provide details
 otherwise. I understand that work that has been developed by another student,
 or by me in collaboration with other students, or by non-students as a result
 of request, solicitation, or payment, may not be submitted for assessment in
 this subject. I understand that submitting for assessment work developed by
 or in collaboration with other students or non-students constitutes Academic
 Misconduct, and may be penalized by mark deductions, or by other penalties
 determined via the University of Melbourne Academic Honesty Policy, as
 described at https://academicintegrity.unimelb.edu.au.
 (2) I also certify that I have not provided a copy of this work in either
 softcopy or hardcopy or any other form to any other student, and nor will I
 do so until after the marks are released. I understand that providing my work
 to other students, regardless of my intention or any undertakings made to me
 by that other student, is also Academic Misconduct.
 (3) I further understand that providing a copy of the assignment specification
 to any form of code authoring or assignment tutoring service, or drawing the
 attention of others to such services and code that may have been made
 available via such a service, may be regarded as Student General Misconduct
 (interfering with the teaching activities of the University and/or inciting
 others to commit Academic Misconduct). I understand that an allegation of
 Student General Misconduct may arise regardless of whether or not I personally
 make use of such solutions or sought benefit from such actions.
 Signed by: Tuan Phong Vu 1266265
 Dated:     03/10/2021, Melbourne
*/
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
/* some #define's from my sample solution ------------------------------------*/
#define BOARD_SIZE          8       // board size
#define ROWS_WITH_PIECES    3       // number of initial rows with pieces
#define CELL_EMPTY          '.'     // empty cell character
#define CELL_BPIECE         'b'     // black piece character
#define CELL_WPIECE         'w'     // white piece character
#define CELL_BTOWER         'B'     // black tower character
#define CELL_WTOWER         'W'     // white tower character
#define COST_PIECE          1       // one piece cost
#define COST_TOWER          3       // one tower cost
#define TREE_DEPTH          3       // minimax tree depth
#define COMP_ACTIONS        10      // number of computed actions
#define INVALID            -100     // arbitrary large number to use as flag
/* one type definition from my sample solution -------------------------------*/
typedef unsigned char board_t[BOARD_SIZE][BOARD_SIZE];  // board type
typedef struct node node_t;
// represent a basic move in its form A1-B2
typedef struct {
   char col_in;
   int row_in;
   char col_out;
   int row_out;
} move_t;
/* We going to represent our n-ary tree using binary tree.
1 parent node N will be pointing to only 1 child, other childs
that is classify as child of that node N will be pointed at
by it previous sibling. This make looping through all the childs
a easier task as well as we do not need to adjust the size of
any array in case we have more childs, we just malloc'ed a new node instead */
struct node {
   board_t board;
   int cost;
   move_t move;
   node_t *child;
   node_t *sibling;
};
/****************************************************************/
void fill_board(board_t A);
void print_board(board_t A);
int char_to_int(char c);
char prom_tower(char cell);
char side_check(int turn);
void board_copy (board_t A, board_t B);
int check_capture (board_t A,int row, int col,char side,int print);
int check_move(move_t *move, char s_cell, char t_cell, char side, int print);
void fill_move(move_t *move, int s_row, int s_col , int t_row, int t_col);
void potential_move(int row, int col, int* next_row, int* next_col, int s);
int apply_move(board_t A, move_t *move, int turn, int start, int cost);
int find_add_child(board_t A, int turn, node_t *node, int cost);
node_t *new_node(board_t A, int cost, move_t move);
node_t *add_childs(node_t* n, board_t A, int cost, move_t move);
node_t *add_sibling(node_t * n,board_t A, int cost, move_t move);
void free_tree(node_t *node);
void loop_add(node_t* n, int turn);
node_t* compute_minimax (node_t* n, int max);
void loop_compute (node_t* n, int max);
int next_move (board_t A, int turn, int board_cost, move_t move);
/****************************************************************/
int
main(int argc, char *argv[]) {
   int turn = 0, board_cost=0, read,i;
   board_t BA;
   move_t move;
   
   printf ("BOARD SIZE: 8x8\n#BLACK PIECES: 12\n#WHITE PIECES: 12\n");
   fill_board(BA);
   print_board(BA);
   
   while (((read = scanf("%c%d-%c%d\n",
               &move.col_in,  &move.row_in,
               &move.col_out, &move.row_out)) == 4) || read == 1)
   {
       turn++;
       if (read == 1)
       {
           if (move.col_in=='A') next_move(BA, turn, board_cost, move);
           else
           {
               for (i=0; i < COMP_ACTIONS; i++)
               {
               board_cost =  next_move(BA, turn, board_cost, move);
               turn++;
               }
           }
       }
       else
       {
         board_cost = apply_move(BA, &move, turn, 1, board_cost);
         printf("BOARD COST: %d\n", board_cost);
         print_board(BA);
       }
       
   }
   return EXIT_SUCCESS;            // exit program with the success code
}
/****************************************************************/
/*First initialization of board state*/
void
fill_board(board_t A) {
   int row, columns;
   char cur_side;
   
   for (row=0; (row+1) <= BOARD_SIZE; row++)
   {
       for (columns=0; (columns + 1) <= BOARD_SIZE; columns++)
       {
           if ((row + 1)> ROWS_WITH_PIECES && (row + 1) < 2*ROWS_WITH_PIECES)
               A[row][columns] = CELL_EMPTY;
           else
           {
               if ((row + 1) <= ROWS_WITH_PIECES) cur_side = CELL_WPIECE;
               else cur_side = CELL_BPIECE;
               if ((row + 1) % 2 == 0)
               {
                   if ((columns+1) % 2 == 0) A[row][columns] = CELL_EMPTY;
                   else A[row][columns] = cur_side;
               }
               else
               {
                   if ((columns+1) % 2 == 0) A[row][columns] = cur_side;
                   else A[row][columns] = CELL_EMPTY;
               }
           }
       }
   }
}
/****************************************************************/
/*Print board A*/
void
print_board(board_t A) {
   int i,j;
   
   printf("     A   B   C   D   E   F   G   H");
   for (i=0; i < BOARD_SIZE; i++)
   {   printf("\n");
       printf("   +---+---+---+---+---+---+---+---+\n");
       printf (" %d |", i+1);
       for (j=0;j<BOARD_SIZE;j++)
       {
           printf(" %c |",A[i][j]);
       }
   }
   printf("\n   +---+---+---+---+---+---+---+---+\n");
 
}
/****************************************************************/
/*Take a character, utilize its ASCII value to return a value that
can be used as in indexing the board (colums)*/
int char_to_int(char c){
   return c - 65;
}
/****************************************************************/
/*Position of next move to next_row, next_col by taking in row,col
and also integer s to indicate the next direction*/
void
potential_move(int row, int col, int* next_row, int* next_col,int s) {
   if (s==1||s==4) *next_row = row-2;
   else *next_row = row+2;
   if (s==4||s==3) *next_col = col-2;
   else *next_col = col+2;
   
}
/****************************************************************/
/*Copy board A into board B*/
void
board_copy(board_t A,board_t B){
   int i,j;
   for (i=0;i<BOARD_SIZE;i++)
   {
       for (j=0;j<BOARD_SIZE;j++) B[i][j]=A[i][j];
   }
}
/****************************************************************/
/*Check if a capture move actually capture something valid*/
int
check_capture (board_t A, int row, int col, char side, int print){
   char cell;
   cell = A[row][col];
   if (cell == CELL_EMPTY || cell == side || cell == prom_tower(side))
   {
       if (print) printf("ERROR: Illegal action.\n");
       return 1;
   }
   return 0;
}
/****************************************************************/
/*Turning a cell into its tower form*/
char
prom_tower(char cell) {
   if (cell == CELL_BPIECE) cell = CELL_BTOWER;
   else cell = CELL_WTOWER;
   return cell;
}
/****************************************************************/
/*Fill move with all the given source row, col and target row, col
note that the parameter taking in the row and col as still at it index form*/
void
fill_move(move_t *move, int s_row, int s_col , int t_row, int t_col) {
   // change row and col from index form to appropreate "move" form
   s_row++;
   s_col+=65;
   t_row++;
   t_col+=65;
   move->row_in = s_row;
   move->col_in = (char)s_col;
   move->row_out = t_row;
   move->col_out = (char)t_col;
}
/****************************************************************/
/*Taking in turn and returning "b" or "w" depend on it is black or white turn*/
char side_check(int turn) {
   char side;
   if (turn % 2 !=0) side = CELL_BPIECE;
   else side = CELL_WPIECE;
   return side;
}
/****************************************************************/
/*Create a new node from the given data utilizing malloc'ed memory*/
node_t *new_node(board_t A, int cost, move_t move) {
   node_t *new_node = malloc(sizeof(node_t));
   assert(new_node);
   
   new_node->sibling = NULL;
   new_node->child = NULL;
   new_node->cost = cost;
   new_node->move = move;
   board_copy(A, new_node->board);
        
   return new_node;
}
/****************************************************************/
/*Add childs to node n if there isn't any, or else
add sibling to the last child of node n*/
node_t
*add_childs(node_t* n, board_t A, int cost, move_t move) {
   if ( n == NULL )
       return NULL;
   if ( n->child )
       return add_sibling(n->child,A, cost, move);
   else
       return (n->child = new_node(A, cost, move));
}
/****************************************************************/
/*Move node pointer n to its last sibling, then add a new node (more sibling)*/
node_t
*add_sibling(node_t *n, board_t A, int cost, move_t move) {
   if ( n == NULL )
       return NULL;
   while (n->sibling)
       n = n->sibling;
   return (n->sibling = new_node(A,cost, move));
}
/****************************************************************/
/*Recursively free the malloc'ed tree*/
void free_tree (node_t *node) {
   if (node == NULL) return;
   free_tree(node->sibling);
   free_tree(node->child);
   free(node);
}
/****************************************************************/
/*Apply move into board A, while taking in turn to checking if all rules hold
-if start == 1 we will print the board as we taking in move and abort
 the whole program if error is found. Else we will just checking and returning
 the new board cost if it is the valid move, INVALID if it is not*/
int apply_move(board_t A, move_t *move, int turn, int start,int cost) {
   
   char s_cell, t_cell, side, cap_cell;
   int  mid_r, mid_c, flag;
     
   side = side_check(turn);
       
   s_cell = A[move->row_in - 1][char_to_int(move->col_in)];
   t_cell = A[move->row_out - 1][char_to_int(move->col_out)];
   
   
   // Here check_move evaluate to a move with distance of 2, we have to
   // check if that move is a valid capture, i.e it actually capture
   // something in between it source and target cell
   // if it is, we modify the board cost
   if ( (flag=check_move(move, s_cell, t_cell, side, start)) == 2 )
   {
       mid_r = ( (move->row_out - 1) + (move->row_in - 1) ) /2;
       mid_c = ( char_to_int(move->col_out) + char_to_int(move->col_in) )/2;
       cap_cell = A[mid_r][mid_c];
       
       if (check_capture(A, mid_r, mid_c, side, start)) flag = 0;
       else
       {
           if (cap_cell == CELL_BPIECE) cost-=COST_PIECE;
           else if (cap_cell == CELL_BTOWER) cost-=COST_TOWER;
           else if (cap_cell == CELL_WPIECE) cost+=COST_PIECE;
           else cost+=COST_TOWER;
       }
   }
   if (flag == 0)
   {
       if (start == 1) exit(EXIT_SUCCESS);
       else return INVALID;
   }
   
   if (start)
   {
       printf("=====================================\n");
       if (start == 2) printf("*** "); // for stage 1 and stage 2
       if (side == CELL_BPIECE) printf("BLACK ACTION #%d: %c%d-%c%d\n",
                                     turn, move->col_in,  move->row_in,
                                           move->col_out, move->row_out);
       else printf("WHITE ACTION #%d: %c%d-%c%d\n",
                                     turn, move->col_in,  move->row_in,
                                           move->col_out, move->row_out);
   }
   
   //Promote tower if it reach to the end of the board and not already a tower
   if (( (move->row_out == 8) || (move->row_out == 1) )
           && s_cell != prom_tower(side))
   {
       s_cell = prom_tower(s_cell);
       if (s_cell == CELL_BTOWER) cost+=2;
       else cost-=2;
   }
   // It is a valid move, change board state accordingly
   if (flag == 2) A[mid_r][mid_c] = CELL_EMPTY;
   A[move->row_in - 1][char_to_int(move->col_in)] = CELL_EMPTY;
   A[move->row_out - 1][char_to_int(move->col_out)] = s_cell;
   
   return cost;
}
/****************************************************************/
/*Check whether a given move is valid and print error if it is told to*/
int
check_move(move_t *move, char s_cell, char t_cell, char side, int print) {
   int dis_r, dis_c;
   dis_r = (move->row_out)-(move->row_in);
   dis_c = abs((char_to_int(move->col_out))-(char_to_int(move->col_in)));
   if (((move->row_in > BOARD_SIZE)||(char_to_int(move->col_in)+1>BOARD_SIZE))
   || ( (move->row_in < 0) || (char_to_int(move->col_in) < 0) ))
   
   {
       if (print) printf("ERROR: Source cell is outside of the board.\n");
       dis_c = 0;
   }
   else if(((move->row_out > BOARD_SIZE)||
               (char_to_int(move->col_out) + 1>BOARD_SIZE))  ||
                   ((move->row_out < 0) || (char_to_int(move->col_out) < 0)))
   {
       if (print) printf("ERROR: Target cell is outside of the board.\n");
       dis_c = 0;
   }
   else if (s_cell == CELL_EMPTY)
   {
       if (print) printf("ERROR: Source cell is empty.\n");
       dis_c = 0;
   }
   else if (t_cell != CELL_EMPTY)
   {
       if (print) printf("ERROR: Target cell is not empty.\n");
       dis_c = 0;
   }
   else if ( !(s_cell == side || s_cell == prom_tower (side)) )
   {
       if (print) printf("ERROR: Source cell holds opponent's piece/tower.\n");
       dis_c = 0;
   }
   //Condition is Black piece can only move upward => negative row distance
   //The opposite apply to White piece
   // Also check if the piece try to move more than 2 squares distance
   else
   {
       if ((abs(dis_r) == dis_c) && (abs(dis_r) <=2) && (dis_c <=2))
       {
           if ((s_cell == CELL_BPIECE && dis_r > 0) ||
               (s_cell == CELL_WPIECE && dis_r < 0))
           {
               if (print) printf("ERROR: Illegal action.\n");
               dis_c = 0;
           }
       }
       else
       {
           if (print) printf("ERROR: Illegal action.\n");
           dis_c = 0;
       }
   }
   return dis_c;
}
/****************************************************************/
/*Given the condition of the board, and turn check for all possible
move using potential_move, try to apply that move, if success, add a child
into node n, return 0 if no child could be added*/
int
find_add_child(board_t A, int turn, node_t *n, int cost) {
   int next_row, next_col, row, col, i, flag=0, t_co = cost; //temporary cost
   move_t move;
   char side = side_check(turn);
   board_t t_Bo; //temporary Board
   // All the temoporary variable is to use to save the
   //information of the board while not alter the current main board A
   
   for(row=0;row<BOARD_SIZE;row++) {
       for (col=0;col<BOARD_SIZE;col++) { // Nested loop to loop through board
           if (A[row][col] == side || A[row][col] == prom_tower(side))
           {
               for(i=1;i<=4;i++)
               {   // Find potential move, fill it into move
                   potential_move(row,col, &next_row, &next_col,i);
                   
                   board_copy(A,t_Bo);
                   fill_move(&move, row, col, next_row, next_col);
                   
                   if ((t_co=apply_move(t_Bo, &move, turn, 0, cost))!=INVALID)
                   {
                   // is it a valid move, so add a childs (or siblings)
                      add_childs(n,t_Bo,t_co, move);
                      flag = 1;
                   }
                   // Since potential move give out the next move with
                   //distance of 2 (capture), divide by 2 give us
                   //a regular move
                   next_row = row + (next_row - row)/2;
                   next_col = col + (next_col - col)/2;
                   // Apply the regular move again to our current board
                   // through a temporary board t_Bo
                   board_copy(A, t_Bo);
                   fill_move(&move, row, col, next_row, next_col);
                   
                   if((t_co=apply_move(t_Bo, &move, turn, 0, cost))!=INVALID)
                   {
                      add_childs(n, t_Bo, t_co, move);
                      flag = 1;
                   }
               }
           }
   }    }
   return flag;
}
/****************************************************************/
/*Looping through all the sibling of a given node to add childs to that node */
void loop_add(node_t* n, int turn) {
   int INT_MINIMAX;
   if (side_check(turn) == CELL_BPIECE) INT_MINIMAX = INT_MIN;
   else INT_MINIMAX = INT_MAX;
   
   // INT_MINIMAX represent cost of boards where no childs (possible move)
   // is found
   
   if (!find_add_child(n->board, turn, n, n->cost)) n->cost = INT_MINIMAX;
   while (n->sibling)
   {
       n = n->sibling;
       if (!find_add_child(n->board, turn, n, n->cost)) n->cost = INT_MINIMAX;
   }
}
/****************************************************************/
/*Take in board state, turn, cost and the last move to compute the next move
and return back the new after change board cost*/
int next_move (board_t A, int turn, int board_cost, move_t move) {
   int max=0;
   node_t *root, *node, *temp;
   // temp is a temporary pointer that we will use to move around our tree
   
   root = new_node(A, board_cost, move);  //Make the root node (level 0)
   
   //This assignment help us not to alter the address of our root
   node = root;
   
   // Finding next move from our current board state (compute level 1 of tree)
   //If no next move can be found, current turn's player lose
   
   if (!find_add_child(node->board,turn, node, node->cost))
   {
       if (side_check(turn) == CELL_BPIECE) printf("WHITE WIN!\n");
       else printf("BLACK WIN!\n");
       exit(EXIT_SUCCESS);
   }
   turn++;
   node = node->child; //Move our node pointer to level 1
   loop_add(node, turn); //Find all childs(level 2) of level 1 childs
   
   // node will now be pointing to the first child of our root
   // temp then will be pointing at the first child of node
   // loop_add then able to add childs to all of temp sibling
   turn++;
   node = root->child;
   temp = node->child;
   loop_add(temp,turn);
   // repeat the procedure for the rest of node sibling
   while (node->sibling)
   {
       node = node->sibling;
       temp = node->child;
       loop_add(temp,turn);
   }
   
   if (side_check(turn) == CELL_BPIECE) max = 1;
   //Since loop_compute will compute the cost of the node childs
   // we will start at level 2 by doin the exact same procedure as how we
   // add child
   node = root->child;
   temp = node->child;
   loop_compute(temp,max);
   while (node->sibling)
   {
       node = node->sibling;
       temp = node->child;
       loop_compute(temp,max);
   }
   // since root only has no sibling we only need to compute_minimax
   node = root->child;
   loop_compute(node, !max);
   root = compute_minimax(root, max);
   
   //Apply move and print board accordingly
   board_cost = apply_move(A, &root->move, turn-2, 2, board_cost);
   free_tree(root);
   printf("BOARD COST: %d\n", board_cost);
   print_board(A);
   return board_cost;
   /**There has been alot of duplicate code in this part but i was
   unable to figure out a method to do it recursively
   while having 2 different functions.*/
}
/****************************************************************/
/*compute the minimax cost of a given node by looking at its child nodes
and return a pointer to the first encounter minimax value*/
node_t* compute_minimax (node_t* n, int max) {
   node_t* childs, *minimax;
   childs = n->child;
   n->cost = childs->cost;
   minimax = childs;
   
   //Assign the minimax cost of node n with the cost of its first child
   // Looping through all the child nodes to see if there is a more extreme
   // value to be saved into the cost of node n
   while (childs->sibling)
   {
       childs = childs->sibling;
       if (max) {
           if (childs->cost > n->cost)
           {
               n->cost = childs->cost;
               minimax = childs;
           }
       }
       else {
           if (childs->cost < n->cost)
           {
               n->cost = childs->cost;
               minimax = childs;
           }
       }
   }
   return minimax;
}
/****************************************************************/
/*apply compute_minimax to all the sibling of node n*/
void loop_compute (node_t* n, int max){
   compute_minimax(n, max);
   while (n->sibling)
   {
       n = n->sibling;
       compute_minimax(n, max);
   }
}
/**************algorithms are fun (duh)*******************/
