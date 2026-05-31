#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>
#include <string>

namespace
{
const int WIDTH = 70;
const int HEIGHT = 70;
} // namespace

int main()
{
  auto screen = ftxui::Screen::Create(ftxui::Dimension::Fixed(WIDTH),
                                      ftxui::Dimension::Fixed(HEIGHT));

  int alternate = 0;
  for (int y = 0; y < HEIGHT; ++y)
  {
    for (int x = 0; x < WIDTH; ++x)
    {
      auto& cell = screen.PixelAt(x, y);
      if (x == 0)
      {
        cell.character = std::to_string(y / 10);
      }
      if (y == 0)
      {
        cell.character = std::to_string(x / 10);
      }
      cell.foreground_color = ftxui::Color::White;
      cell.background_color = ftxui::Color::Black;
      if (alternate)
      {
        cell.background_color = ftxui::Color::RGB(0, 0, 255);
      }
      alternate = !alternate;
    }
    alternate = !alternate;
  }

  // Access a specific pixel at (10, 5)
  auto& cell = screen.PixelAt(11, 5);

  // Set properties of the cell.
  cell.character = "X";
  cell.foreground_color = ftxui::Color::Red;
  cell.background_color = ftxui::Color::RGB(0, 255, 0);
  cell.bold = true; // Set bold style
  screen.Print();   // Print the screen to the terminal

  std::cout << "resolution: width=" << WIDTH << " height=" << HEIGHT
            << std::endl;

  return 0;
}
