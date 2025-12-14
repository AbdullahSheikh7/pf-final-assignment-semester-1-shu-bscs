#include "select_list.h"

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

    if (c == 27)
    {
      int c2 = getch();
      int c3 = getch();

      if (c3 == 'A')
      {
        selected--;
        if (selected < 0)
        {
          selected = length - 1;
        }
      }
      if (c3 == 'B')
      {
        selected++;
        if (selected > (length - 1))
        {
          selected = 0;
        }
      }

      cout << "\033[3A";
    }

    if (c == 10)
    {
      ok = true;
    }
  } while (!ok);
  cout << "\033[?25h";

  return options[selected];
}
