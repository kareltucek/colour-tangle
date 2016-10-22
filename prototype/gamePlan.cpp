#include <vector>
#include <string>

#define X 0
#define Y 1

#define RAD 0

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

using namespace std;
struct Plan
{
  typedef vector<char> stateColumn;
  typedef vector<stateColumn> stateMatrix;
  typedef vector<stateMatrix> statePlan;
  vector<vector<vector<char> > > plan;
  vector<vector<int> > state;
  int x;
  int y;
  int x2;
  int y2;
  int width;
  int height;
  int stateMod;
  int rad;
  bool restricted;

  Plan(): state(), plan(), x(0), y(0), stateMod(0)
  {
  }

  void loadFromFile(string fn)
  {
    FILE* fp = fopen(fn.c_str(), "r");

    width = 10;
    height = 10;
    rad = 0;
    restricted = true;

    for(int i = 0; i < 10; i++)
    {
      plan.push_back(stateMatrix());
      for(int a = 0; a < 40; a++)
      {
        plan[i].push_back(stateColumn());
        for(int b = 0; b < 40; b++)
        {
          plan[i][a].push_back('?');
        }
      }
    }

    for(int a = 0; a < 40; a++)
    {
      state.push_back(vector<int>());
      for(int b = 0; b < 40; b++)
      {
        state[a].push_back(0);
      }
    }

    int myx = 0, myy = 0;
    int mylevel = 0;
    int skip = 0;

    while(true)
    {
      char a = fgetc(fp);
      switch(a)
      {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          stateMod = max(stateMod, (int)a-48+1);
          state[myx][myy] = (int)a-48;
          break;

        case '\n':
          myy++;
          skip = (myy%2)+1;
          myx=0;

          //yep, go on

        case ' ':
          mylevel = 0;

          if(skip > 0)
          {
            skip--;
          }
          else
          {
            myx++;
          }
          break;


        case 'X':
          x = myx;
          y = myy;
          break;

        case 'Y':
          x2 = myx;
          y2 = myy;
          break;

        case EOF:
          return;
          break;

        default:
          plan[mylevel][myx][myy] = a;
          mylevel++;
          break;
      }
    }
  }

  void show()
  {
    printf("\nradius: %i; restricted: %i\n", rad, restricted);
    for(int myy = 0; myy < height; myy++)
    {
      if(myy % 2 == 1)
        printf(" ");
      for(int myx = 0; myx < width; myx++)
      {
        if(x == myx && y == myy)
          printf("X ");
        else if(x2 == myx && y2 == myy)
          printf("Y ");
        else
        {
          char c = plan[state[myx][myy]][myx][myy];
          if(c == 'r' || c == 'R')
            printf(ANSI_COLOR_RED);
          if(c == 'b' || c == 'B')
            printf(ANSI_COLOR_BLUE);
          if(c == 'g' || c == 'G')
            printf(ANSI_COLOR_GREEN);
          if(isupper(c))
            printf("# ", c);
          else
            printf(". ");
          printf(ANSI_COLOR_RESET);
        }
      }
      printf("\n");
    }
  }

  int decodeDir(int x, int y, int dir, int coord)
  {
    int mx;
    int my;
    switch(dir % 12)
    {
      case 0:
        mx = 1;
        my = -1;
        break;
      case 2:
        mx = 1;
        my = 0;
        break;
      case 4:
        mx = 1;
        my = 1;
        break;
      case 6:
        mx = 0;
        my = 1;
        break;
      case 8:
        mx = -1;
        my = 0;
        break;
      case 10:
        mx = 0;
        my = -1;
        break;
    }

    int mod = (y % 2 == 0 && my != 0) ? -1 : 0;
    x = x + mx + mod;
    y = y + my;

    if(coord == X)
      return x;
    else
      return y;
  }

  bool valid(int x, int y)
  {
    char c = plan[state[x][y]][x][y];
    return !isupper(c);
  }

  void step(char c)
  {
    int dir = 0;
    int pl = 0;
    switch(c)
    {
      case 'E':
        pl = 1;
        dir = 0;
        break;
      case 'D':
        pl = 1;
        dir = 2;
        break;
      case 'X':
        pl = 1;
        dir = 4;
        break;
      case 'Z':
        pl = 1;
        dir = 6;
        break;
      case 'A':
        pl = 1;
        dir = 8;
        break;
      case 'W':
        pl = 1;
        dir = 10;
        break;
      case 'e':
        dir = 0;
        break;
      case 'd':
        dir = 2;
        break;
      case 'x':
        dir = 4;
        break;
      case 'z':
        dir = 6;
        break;
      case 'a':
        dir = 8;
        break;
      case 'w':
        dir = 10;
        break;
      case '0':
        rad = 0;
        return;
      case '1':
        rad = 1;
        return;
      case '2':
        rad = 2;
        return;
      case 'i':
        restricted = !restricted;
        return;
      default:
        dir = 20;
        break;
    }
    if(dir == 20)
      return;
    int& px = pl == 0 ? x : x2;
    int& py = pl == 0 ? y : y2;

    int nx = decodeDir(px, py, dir, X);
    int ny = decodeDir(px, py, dir, Y);
    if(!valid(nx, ny))
      return;
    else
    {
      px = nx;
      py = ny;
    }

    int initState = state[px][py];
    for(int i = -rad; i < rad+1; i++)
    {
      int& st = state[decodeDir(px, py, dir+i+6, X)][decodeDir(px, py, dir+i*2+6, Y)];
      if(!restricted || st == initState)
        st = (st + 1) % stateMod;
    }
  }
};

char nextStep()
{
  system("/bin/stty raw");
  char c = getchar();
  system("/bin/stty cooked");
  return c;
}

int main(int argc, char* argv[])
{
  string in, out;
  if(argc < 2)
    in = "data.txt";
  else
    in = string(argv[1]);

  system ("/bin/stty raw");
  Plan p;
  p.loadFromFile(in);
  p.show();
  char c = nextStep();
  while(c != EOF)
  {
    p.step(c);
    p.show();
    c = nextStep();
    if(c == 's')
      break;
  }
  return 0;
}


