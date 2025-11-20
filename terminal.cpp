#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <random>
#include <unistd.h>
#include <conio.h>
#include <windows.h>
#include <utility>

using namespace std;
using namespace this_thread;
using namespace chrono;

int generateRandom(int start, int end)
{
  random_device rd;
  mt19937 gen(rd());

  uniform_int_distribution<> distrib(0, 9);

  int randomNumber = distrib(gen);

  return randomNumber;
}

void wait(int ms)
{
  sleep_for(milliseconds(ms));
}

pair<int, int> getTerminalSize()
{
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
  {
    // Agar error aaye to default size return karo
    return {80, 25};
  }

  int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

  return {columns, rows};
}

string selectList(string options[], int length)
{
  bool ok = false;
  int selected = 0;
  cout << "\033[?25l";
  do
  {
    for (int i = 0; i < length; i++)
    {
      if (i == selected)
      {
        cout << ">" << flush;
      }
      else
      {
        cout << " " << flush;
      }
      cout << " " << i + 1 << "." << " " << options[i] << endl
           << flush;
    }

    int c = getch();

    if (c == 72)
    {
      selected--;
      if (selected < 0)
      {
        selected = length - 1;
      }
    }
    if (c == 80)
    {
      selected++;
      if (selected > (length - 1))
      {
        selected = 0;
      }
    }

    if (c == 13)
    {
      ok = true;
    }
    else
    {
      cout << "\033[3A";
    }
  } while (!ok);

  cout << "\033[?25h";

  return options[selected];
}

int main()
{
  int loading = 0;
  auto [width, height] = getTerminalSize();
  string titleLines[9] = {
      "               _____ _                 _   ____      _ _       __             ",
      "              |  ___| | ___   ___   __| | |  _ \\ ___| (_) ___ / _|            ",
      "              | |_  | |/ _ \\ / _ \\ / _` | | |_) / _ \\ | |/ _ \\ |_             ",
      "              |  _| | | (_) | (_) | (_| | |  _ <  __/ | |  __/  _|            ",
      "  ____        |_|   |_|\\___/ \\___/ \\__,_| |_| \\_\\___|_|_|\\___|_|_             ",
      " |  _ \\ ___  ___  ___  _   _ _ __ ___ ___  |_   _| __ __ _  ___| | _____ _ __ ",
      " | |_) / _ \\/ __|/ _ \\| | | | '__/ __/ _ \\   | || '__/ _` |/ __| |/ / _ \\ '__|",
      " |  _ <  __/\\__ \\ (_) | |_| | | | (_|  __/   | || | | (_| | (__|   <  __/ |   ",
      " |_| \\_\\___||___/\\___/ \\__,_|_|  \\___\\___|   |_||_|  \\__,_|\\___|_|\\_\\___|_|   "};

  cout << "\033c";
  cout << "\033[" << static_cast<int>((height - 9) / 2) << "B";
  for (int i = 0; i < 9; i++)
    cout << "\033[" << static_cast<int>((width - 79) / 2) << "C" << titleLines[i] << endl;

  cout << endl
       << "\033[?25l";
  for (int i = 0; i < 15; i++)
  {
    loading += 7;
    wait(200);
    cout << "\033[" << static_cast<int>((width - 15) / 2) + i << "C" << "|" << flush;
    cout << endl
         << "\033[" << static_cast<int>((width - 13) / 2) << "C" << "Loading..." << loading << endl;
    cout << "\033[2A";
  }
  cout << "\033[?25h";

  cout << "\033c";

  string options[3] = {"Option A", "Option B", "Option C"};

  cout << "Welcome to my app" << endl;

  string choice = selectList(options, 3);

  cout << choice << endl;

  // cout << endl << "\033[" << static_cast<int>((width - 13) / 2) << "C" << "Loading..." << loading << endl;

  // int randomNumber = generateRandom(0, 9);

  // cout << randomNumber << endl;

  // char key = getch();

  // cout << key << endl;

  return 0;
}
