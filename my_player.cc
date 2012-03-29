#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

//Variablen

// Moegliche Muehlen auf einem Muehle-Brett
char muehlen[16][3] = {{'A','B','C'},{'D','E','F'},{'G','H','I'},
  {'J','K','L'},{'M','N','O'},{'P','Q','R'},{'S','T','U'},{'V','W','X'},
  {'A','J','V'},{'D','K','S'},{'G','L','P'},{'B','E','H'},{'Q','T','W'},
  {'I','M','R'},{'F','N','U'},{'C','O','X'}};
// Moegliche Zuege; Verbindungen zwischen Positionen
char moegliche_wege[32][2] = {{'A','B'},{'B','C'},{'C','O'},{'O','X'},
  {'X','W'},{'W','V'},{'V','J'},{'J','A'},{'D','E'},{'E','F'},
  {'F','N'},{'N','U'},{'U','T'},{'T','S'},{'S','K'},{'K','D'},
  {'G','H'},{'H','I'},{'I','M'},{'M','R'},{'R','Q'},{'Q','P'},
  {'P','L'},{'L','G'},{'B','E'},{'E','H'},{'M','N'},{'N','O'},
  {'J','K'},{'K','L'},{'Q','T'},{'T','W'}};
char priority_board[12]={'E','K','N','T','B','Q','W','J','H','M','Q','L'}; // innerer kreuzring bevorzugt!
char board[24];

unsigned current_player;
unsigned unplaced_pieces[2];
char newline;
int matches;
int treffer;
char weggefunden[4];

static char num2sym(char input)
{
  switch (input) {
    case '0':
      return 'X';
    case '1':
      return 'O';
    case '.':
      return ' ';
    default:
      return input;
  }
}


int muehlenbrett(){

    printf("\n");
    printf("Current board situation on the left,\n");
    printf("letters for identifying the places on the right:\n");
    printf("\n");
    printf("%c----------%c----------%c            A----------B----------C\n",
         num2sym(board[0]), num2sym(board[1]), num2sym(board[2]));
    printf("|          |          |            |          |          |\n");
    printf("|   %c------%c------%c   |            |   D------E------F   |\n",
         num2sym(board[3]), num2sym(board[4]), num2sym(board[5]));
    printf("|   |      |      |   |            |   |      |      |   |\n");
    printf("|   |   %c--%c--%c   |   |            |   |   G--H--I   |   |\n",
         num2sym(board[6]), num2sym(board[7]), num2sym(board[8]));
    printf("|   |   |     |   |   |            |   |   |     |   |   |\n");
    printf("%c---%c---%c     %c---%c---%c            J---K---L     M---N---O\n",
         num2sym(board[9]), num2sym(board[10]), num2sym(board[11]),
         num2sym(board[12]), num2sym(board[13]), num2sym(board[14]));
    printf("|   |   |     |   |   |            |   |   |     |   |   |\n");
    printf("|   |   %c--%c--%c   |   |            |   |   P--Q--R   |   |\n",
         num2sym(board[15]), num2sym(board[16]), num2sym(board[17]));
    printf("|   |      |      |   |            |   |      |      |   |\n");
    printf("|   %c------%c------%c   |            |   S------T------U   |\n",
         num2sym(board[18]), num2sym(board[19]), num2sym(board[20]));
    printf("|          |          |            |          |          |\n");
    printf("%c----------%c----------%c            V----------W----------X\n",
         num2sym(board[21]), num2sym(board[22]), num2sym(board[23]));
    printf("\n");

    return 0;
}

//Funktionen

char int2sym(int player) {
  switch(player) {
    case 0: return '0';
    case 1: return '1';
   default: return '.';
  }
}
char int2char(int random_integer) {
  char umwandlung = random_integer+65;
  return umwandlung;
}

int char2int(char random_character) {
  int umwandlung = random_character-65;
  return umwandlung;
}

char random_piece_of_color(char col) {
    char pos;
    int random_integer, gesetzt = 0;
    
    while (gesetzt == 0) {
        random_integer = random()%24;
        
        if(board[random_integer] == col) {
            pos = int2char(random_integer);
            gesetzt = 1;
        }
    }
    return pos;
}

char steinesetzen() {
    for (int i=0;i<12;i++){
        if(priority_board[i] == '.'){
            return priority_board[i];
        }
    }
    return random_piece_of_color('.');
}

char is_muehle_of_color(char* muehle, char col){
    //Gibt zurueck welchen Stein ich wegnehmen kann
    char muehlenbelegung[4] = "   ";
    char referenz[4] = "   ";

    //Gibt Leerzeichen zurueck falls die muehle eine muehle ist
    for (int j = 0; j<3; j++) {
        muehlenbelegung[j] = board[char2int(muehle[j])];
        referenz[j] = col;
    }
    if(strcmp(muehlenbelegung, referenz) == 0)
        return ' ';

    // finde stelle, die nicht frei ist und v.d. farbe col ist gebe sie zurueck
    for (int i=0; i<3; i++) {
        if(muehlenbelegung[i] == col)
            return muehle[i];
    }
    return ' ';
}

bool is_in_muehle(char pos, char col) {
    for(int i=0; i<16; i++) {
      for(int j=0; j<3; j++) {
        if(muehlen[i][j] == pos && board[char2int(pos)] == col)
          if(is_muehle_of_color(muehlen[i], col) == ' ')
            return true;
      }
    }
    return false;
}

char killstein() {
    // durchsucht alle mgl. muehlen und entf. aus unkompletten muehlen
    char enemy_color = int2sym((current_player+1)%2);

    for(int i=0; i<32; i++)
      if(board[i] == enemy_color && !is_in_muehle(int2char(i), enemy_color)) {
        printf("Ist ausserhalb gegn. muehle: %c\n", int2char(i));
        return int2char(i);
      }
    return ' ';
}

char close_piece_of_color(char from, char col) {
  char to;
  for(int i=0;i<32;i++){
    for (int j=0; j<2; j++) {
      to = moegliche_wege[i][(j+1)%2];
      if(moegliche_wege[i][j] == from && board[char2int(to)] == col) {
        return to;
      }
    }
  }
  return -1;
}

void close_pieces_of_color(char from, char col, char* list) {
  char to;
  int k=0;
  for(int i=0;i<32;i++){
    for (int j=0; j<2; j++) {
      to = moegliche_wege[i][(j+1)%2];
      if(moegliche_wege[i][j] == from && board[char2int(to)] == col) {
        list[k] = to;
        k++;
      }
    }
  }
}

void set_str(char* a, char* b) {
  int i=0;
  while(a[i] != '\0' && b[i] != '\0') {
    b[i] = a[i];
    i++;
  }
}

int check_muehlen(int player, char* piece_put, char* muehle) {
    // gibt 1 zurueck wenn muehle vervollstaendigt werden koennte, sonst 0
    char muehlenbelegung[4] = "   ";
    // suche nach muehlen
    for (int i = 0; i<16; i++) {
        for (int j = 0; j<3; j++) {
            muehlenbelegung[j] = board[char2int(muehlen[i][j])];
        }
        printf("Checke muehle mit {%c, %c, %c}\n",
            muehlenbelegung[0], muehlenbelegung[1], muehlenbelegung[2]);
        if (current_player == 0) {
            if (strcmp(muehlenbelegung,".11") == 0) {
                *piece_put = muehlen[i][0];
                set_str(muehlen[i], muehle);
            } 
              else
            if (strcmp(muehlenbelegung,"1.1") == 0) {
                *piece_put = muehlen[i][1];
                set_str(muehlen[i], muehle);
            } else
            if (strcmp(muehlenbelegung,"11.") == 0) {
                *piece_put = muehlen[i][2];
                set_str(muehlen[i], muehle);
            } else
            if (strcmp(muehlenbelegung,".00") == 0) {
                *piece_put = muehlen[i][0];
                set_str(muehlen[i], muehle);
                return 1;
            } else
            if (strcmp(muehlenbelegung,"0.0") == 0) {
                *piece_put = muehlen[i][1];
                set_str(muehlen[i], muehle);
                return 1;
            } else
            if (strcmp(muehlenbelegung,"00.") == 0) {
                *piece_put = muehlen[i][2];
                set_str(muehlen[i], muehle);
                return 1;
            }
        } else {
            if (strcmp(muehlenbelegung,".00") == 0) {
                *piece_put = muehlen[i][0];
                set_str(muehlen[i], muehle);
            } else
            if (strcmp(muehlenbelegung,"0.0") == 0) {
                *piece_put = muehlen[i][1];
                set_str(muehlen[i], muehle);
            } else
            if (strcmp(muehlenbelegung,"00.") == 0) {
                *piece_put = muehlen[i][2];
                set_str(muehlen[i], muehle);
            } else
            if (strcmp(muehlenbelegung,".11") == 0) {
                *piece_put = muehlen[i][0];
                set_str(muehlen[i], muehle);
                return 1;
            } else
            if (strcmp(muehlenbelegung,"1.1") == 0) {
                *piece_put = muehlen[i][1];
                set_str(muehlen[i], muehle);
                return 1;
            } else
            if (strcmp(muehlenbelegung,"11.") == 0) {
                *piece_put = muehlen[i][2];
                set_str(muehlen[i], muehle);
                return 1;
            }
        }
    }
    return 0;
}

bool in_str(char* s, char c) {
  int i=0;
  while(s[i]!='\0') {
    if(s[i] == c)
      return true;
    i++;
  }
  return false;
}

int muehlen_move_check(char* piece_move, char* piece_put) {
    // schliesst muehlen durch bewegen
    // gibt 1 zurueck wenn legaler zug mgl., sonst 0
    char tmp_put = ' ';
    char tmp_move = ' ';
    char close_pieces[5] = "    ";
    char my_color = int2sym(current_player);
    char this_muehle[4] = "   ";

    if(check_muehlen(current_player, &tmp_put, this_muehle) == 1) {
        // moeglicherweise schliessbare muehlen gefunden
        close_pieces_of_color(tmp_put, my_color, close_pieces);
        // suche nach steinen, die hineingeschoben werden koennen!
        printf("Close pieces: %s, this_muehle: %s\n", close_pieces, this_muehle);
        for(int i=0; i<5; i++) {
            if(close_pieces[i] == ' ') return 0; // muss existieren
            if(!in_str(this_muehle, close_pieces[i])) {// darf nicht in this_muehle sein
              printf("%c is ok to move\n", close_pieces[i]);
              tmp_move = close_pieces[i]; // oll korrect
              break;
            }
        }
    }
    if(tmp_put == ' ' || tmp_move == ' ') return 0;
    // Ergebnisse zurückgeben
    *piece_move = tmp_move;
    *piece_put = tmp_put;
    return 1;
}

int muehlen_check(char* piece_kill) {
  char piece_put = -1;
  char k[4] = "   ";
  char this_muehle[4] = "   ";

    if(check_muehlen(current_player, &piece_put, this_muehle) == 1) {
        *piece_kill = killstein();
    }
  if(piece_put >= 0) {
    printf("Muehle gefunden (%c) bei {%c, %c, %c}\n", piece_put, k[0], k[1], k[2]);
    if(*piece_kill != ' ')
      printf("Nehme weg: %c\n", *piece_kill);
  }

  return piece_put;
}

char steineziehen(char* piece_move) {
  char from, to=' ';
  bool no_hit = true;

  while(no_hit) { // solange kein feld vom player besetzt...
    from = int2char(random()%24);
    printf("Boardposition ABCDEFGHIJKLMNOPQRSTUVWX\n              ");
    for(int i = 0; i<24; i++) {
        printf("%c", board[i]);
    }
    printf("\n");
    printf("Ich bin Player %c und ich tippe auf....... %c\n", int2sym(current_player), from);
    if(board[char2int(from)] == int2sym(current_player)) { // feld vom player besetzt
        printf("Auf %c liegt ein Stein von mir (%c)\n", from, int2sym(current_player));
        // durchsuche board nach moeglichen zielpositionen
        // versucht eine Mühle auf Zwang zu schließen obwohl da kein Stein liegt
        to = close_piece_of_color(from, '.');
        *piece_move = from;
        printf("Verschiebe von %c nach %c\n", *piece_move, to);
        no_hit = to < 0;
    }
  }
  return to;
}


int main(void)
{
  char piece_move, piece_put, piece_kill;

  // Unix-Pipes oeffnen
  /* current game state is delivered via file descriptor 3 */
  FILE *state_input = fdopen(3, "r");
  /* we output our move via file descriptor 4 */
  FILE *move_output = fdopen(4, "w");

  if (!state_input || !move_output) {
    fprintf(stderr, "error accessing file descriptors\n");
    exit(1);
  }
  srandom(time(NULL));


  while(1) { 
    //Spielzustaende und Board holen
    matches = fscanf(state_input, "%u %u %u%c", &current_player,
        &unplaced_pieces[0], &unplaced_pieces[1], &newline);
    if(matches !=  4) {
      fprintf(stderr, "error while reading the game state\n");
      break;
    }

    matches = fscanf(state_input, "%24c%c", board, &newline);
    if(matches !=  2) {
      fprintf(stderr, "error while reading the board state\n");
      break;
    }
    muehlenbrett();

    // Zuerst auf moegliche Muehlen reagieren und ansonsten zufaellig Stein setzen
    if(unplaced_pieces[current_player]>0) {
      printf("mache muehlencheck...\n");
      piece_move = piece_kill = ' ';
      piece_put = muehlen_check(&piece_kill);

      if(piece_put < 0) { // keine Muehle gefunden
        printf("keine Muehle gefunden...setze stein\n");
        piece_put = steinesetzen();
      }
    } else {
      //Alle Steine gesetzt...jetzt werden moegliche Wege gesucht
      piece_kill= ' ';
      if(muehlen_move_check(&piece_move, &piece_put) == 0)
          piece_put = steineziehen(&piece_move); 
      else
          piece_kill = killstein();
    }
    printf("Player %i does '%c%c%c'\n", current_player, piece_move, piece_put, piece_kill);
    //Rueckgabe aller Werte an den MCP
    fprintf(move_output, "%c%c%c\n", piece_move, piece_put, piece_kill); 
    fflush(move_output);
  }	
  return 0;
}
