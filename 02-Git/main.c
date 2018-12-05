#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum OBJECT
{
   OBJECT_ROCK,
   OBJECT_PAPER,
   OBJECT_SCISSORS,
   OBJECT_MAX,
   OBJECT_INVALID
};

static char * object_names[] =
{
   "rock",
   "paper",
   "scissors"
};

static char behavior[OBJECT_MAX][OBJECT_MAX] = {
   /*OBJECT_ROCK*/
   {
      0,/*against OBJECT_ROCK: draw*/
      -1,/*against OBJECT_PAPER: lose*/
      1,/*against OBJECT_SCISSORS: win*/
   },
   /*OBJECT_PAPER*/
   {
      1,/*against OBJECT_ROCK: win*/
      0,/*against OBJECT_PAPER: draw*/
      -1,/*against OBJECT_SCISSORS: lose*/
   },
   /*OBJECT_SCISSORS*/
   {
      -1,/*against OBJECT_ROCK: lose*/
      1,/*against OBJECT_PAPER: win*/
      0,/*against OBJECT_SCISSORS: draw*/
   }
};

int main(int argc, char * argv[])
{
   int letter;
   enum OBJECT pc_choose;
   enum OBJECT h_choose;

   srand(time(NULL));

   do {
      printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
      letter = getchar();
      switch(letter)
      {
         case 'r':
         case 'R':
            h_choose = OBJECT_ROCK;
            break;
         case 'p':
         case 'P':
            h_choose = OBJECT_PAPER;
            break;
         case 's':
         case 'S':
            h_choose = OBJECT_SCISSORS;
            break;
         default:
            h_choose = OBJECT_INVALID;
            break;
      }
   } while(h_choose == OBJECT_INVALID);

   pc_choose = rand() % OBJECT_MAX;
   printf("You choose %s, I choose %s\n", object_names[h_choose],
                                          object_names[pc_choose]);

   switch(behavior[h_choose][pc_choose])
   {
      case -1:
         printf("I win: %s beats %s\n", object_names[pc_choose],
                                        object_names[h_choose]);
         break;
      case 0:
         printf("There is no winner, there is draw.\n");
         break;
      case 1:
         printf("You win: %s beats %s\n", object_names[h_choose],
                                          object_names[pc_choose]);
         break;
   }
   return 0;
}