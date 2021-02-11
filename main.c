#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char hra[3][3];
char ciel[3][3];
struct node *bot = NULL, *top = NULL;   /// pointre v closed list


struct node
{
    char puzzle[3][3];
    struct node *left, *right, *up, *down, *parent, *next, *nextstack;
    int heuristicv;
    int depth;
    char instance_of;

}*start = NULL;


int calman(char puzzle[3][3],int riad, int stlp)
{
    for (int i = 0; i < 3; i ++)
        for (int j = 0; j < 3; j ++)
            if(puzzle[riad][stlp] == ciel[i][j])
                return abs(riad-i)+abs(stlp-j);
}


int end(struct node* root)
{
    int man = 0;
    int h = 0;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0 ; j < 3; j++)
        {
            if(root ->puzzle[i][j] != ciel[i][j]  && root->puzzle[i][j] != '*')
            {
                h++;
                /// man += calman(root->puzzle,i,j);     výpocet manhattan distance
            }
        }
    }
///return man;  manhattan distance

    return h;
}


void get_xy(int *riad, int *stlp, char puzzle[3][3])        /// funkcia vráti miesto znaku *
{
    for (int i = 0; i < 3; i++ )
        for( int j = 0; j < 3; j++)
        {
            if (puzzle [i][j] == '*')
            {
                *riad = i;
                *stlp = j;
                return;
            }
        }
}

/*int heuristic_value(char puzzle[3][3], char puz[3][3])
{
    for (int i = 0; i < 3; i ++)
        for (int j = 0; j < 3; j ++)
            if (puzzle[i][j] != puz[i][j] )
                return 1;
    return 0;
}*/

struct node *vytvor_start (struct node *root)
{
    struct node *temp  =  (struct node *)malloc(sizeof(struct node));
    memcpy(temp -> puzzle, hra, 9);
    temp -> parent = NULL;
    temp -> depth = 0;
    temp->next = NULL;
    temp->instance_of = 0;
    start = temp;
    start -> heuristicv = end(start);
    return temp;
};

struct node *vytvor_novy(char smer,struct node *root,int riad, int stlp)        /// vytvorím nové uzly, ošetrím smere posunu
{
    char tmp;
    struct node *uzol = (struct node *)malloc(sizeof(struct node));
    uzol->parent = root;
    uzol->depth = root->depth+1;
    uzol->left = NULL;
    uzol->right = NULL;
    uzol->right = NULL;
    uzol->up = NULL;
    uzol->next = NULL;
    memcpy(uzol->puzzle, root->puzzle, 9);


    if (smer == 'l')        /// posun dolava
    {
        tmp = uzol->puzzle[riad][stlp-1];
        uzol->puzzle[riad][stlp-1] = '*';
        uzol->puzzle[riad][stlp] = tmp;
        uzol->heuristicv = uzol->depth + end(uzol->puzzle);
        uzol->instance_of = 'l';

        return uzol;
    }

    if (smer == 'r')        /// posun doprava
    {
        tmp = uzol->puzzle[riad][stlp+1];
        uzol->puzzle[riad][stlp+1] = '*';
        uzol->puzzle[riad][stlp] = tmp;
        uzol->heuristicv = uzol->depth + end(uzol->puzzle);
        uzol->instance_of = 'r';

        return uzol;
    }

    if (smer == 'u')        /// posun hore
    {
        tmp = uzol->puzzle[riad-1][stlp];
        uzol->puzzle[riad-1][stlp] = '*';
        uzol->puzzle[riad][stlp] = tmp;
        uzol->heuristicv = uzol->depth + end(uzol->puzzle);
        uzol->instance_of = 'u';

        return uzol;
    }

    if (smer == 'd')        /// posun dole
    {
        tmp = uzol->puzzle[riad+1][stlp];
        uzol->puzzle[riad+1][stlp] = '*';
        uzol->puzzle[riad][stlp] = tmp;
        uzol->heuristicv = uzol->depth + end(uzol->puzzle);
        uzol->instance_of = 'd';

        return uzol;
    }
}




void print_queue()      /// výpis cesty ku koneènému stavu
{

    while(top != NULL)
    {
        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
                printf("%c",top->puzzle[i][j]);
            printf("\n");
        }
        printf("\n");
        printf("|\n");
        printf("|\n");
        printf("|\n");
        printf("v\n");
        top = top->parent;
    }
    free(start);
    free(bot);
    exit(0);
    return;
}

void enstack(struct node *popped)       /// uzol ulozim do closed listu
{

    if (bot == NULL)
    {
        bot = top = popped;
        bot->nextstack = top->nextstack = NULL;
    }

    else
    {

        top->nextstack = popped;
        top = popped;
        top->nextstack = NULL;
    }
    if (end(popped) == 0)
    {
        printf("|\n");
        printf("|\n");
        printf("|\n");
        printf("v\n");
        print_queue();
    }
    return;
}


void insert_queue(struct node *uzol)            /// uzol ulozim do open listu
{
    if (end(uzol) == 0)
    {
        enstack(uzol);
        return;
    }

    struct node *loop, *help;
    help = start;
    loop  = start->next;
    while(loop != NULL && uzol->heuristicv > loop ->heuristicv )
    {
        help = loop;
        loop = loop->next;
    }

    help->next = uzol;
    uzol->next = loop;
    return;
}

void dequeue(struct node *topop)
{

    struct node* help = start->next;

    enstack(topop);
    start = help;

    return;
}


void generate(struct node *parent)  ///vygenerujem vsetky mozne smere/uzly
{
    struct node *delit, *help;
    int riad, stlp;
    get_xy(&riad,&stlp,parent->puzzle);
    if ( stlp-1 >= 0 )
    {
        if (parent->instance_of != 'r')
        {
            parent->left = vytvor_novy('l',parent,riad,stlp);
            insert_queue(parent->left);
        }
    }

    if (stlp+1 <= 2 )
    {
        if (parent->instance_of != 'l')
        {
            parent->right = vytvor_novy('r',parent,riad,stlp);
            insert_queue(parent->right);
        }
    }

    if ( riad-1 >= 0 )
    {
        if (parent->instance_of != 'd')
        {
            parent->up = vytvor_novy('u',parent,riad,stlp);
            insert_queue(parent->up);
        }
    }
    if ( riad+1 <= 2 )
    {
        if (parent->instance_of != 'u')
        {
            parent->down = vytvor_novy('d',parent,riad,stlp);
            insert_queue(parent->down);
        }
    }
    dequeue(parent);
    generate(start);

    return;
}

int main()
{

    char c,priaz;
    FILE *f = fopen ("puzzle.txt","r");
    struct node *root = NULL;

    printf("Zaciatocna tabulka\n");

    for(int i = 0; i < 3; i++)
        for(int j = 0; j <= 3; j++)
        {
            c = fgetc(f);
            if (c != '\n')
                hra[i][j] = c;
        }

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
            printf("%c",hra[i][j]);
        printf("\n");
    }

    c = fgetc(f);
    printf("\nKonecna tabulka\n");
    for(int i = 0; i < 3; i++)
        for(int j = 0; j <= 3; j++)
        {
            c = fgetc(f);
            if (c != '\n')
                ciel[i][j] = c;
        }

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
            printf("%c",ciel[i][j]);
        printf("\n");
    }
    printf("\n");
    printf("\n\n");
    root = vytvor_start(root);
    generate(root);

    return 0;
}
