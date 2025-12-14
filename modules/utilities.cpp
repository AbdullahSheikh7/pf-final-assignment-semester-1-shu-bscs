#include "utilities.h"

void splashScreen()
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
    cout << "\033[" << static_cast<int>((width - 15) / 2) + i << "C" << "█" << flush;
    cout << endl
         << "\033[" << static_cast<int>((width - 13) / 2) << "C" << "Loading..." << loading << endl;
    cout << "\033[2A";
  }
  cout << "\033[?25h";

  cout << "\033c";
}

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
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return {w.ws_col, w.ws_row};
}

string getCurrentTimestamp()
{
  auto now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);
  char buffer[20];
  std::tm *timeInfo = std::localtime(&t);
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);
  return string(buffer);
}

char getch()
{
  char buf = 0;
  struct termios old = {0};
  tcgetattr(STDIN_FILENO, &old);
  struct termios newt = old;
  newt.c_lflag &= ~ICANON;
  newt.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  read(STDIN_FILENO, &buf, 1);
  tcsetattr(STDIN_FILENO, TCSANOW, &old);
  return buf;
}

string maskPasswordInput()
{
  string password;
  termios oldt;
  tcgetattr(STDIN_FILENO, &oldt);
  termios newt = oldt;
  newt.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  getline(cin, password);
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  cout << endl;
  return password;
}

void clearScreen()
{
  cout << "\033c";
}
