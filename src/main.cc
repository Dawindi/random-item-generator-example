#include <cstdio>
#include <exception>
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>
#include <string>

namespace
{
const int WIDTH = 70;
const int HEIGHT = 70;
const int TENS_DIGIT_DIVISOR = 10;
const int MAX_RGB_COMPONENT = 255;
const int SAMPLE_PIXEL_X = 11;
const int SAMPLE_PIXEL_Y = 5;
} // namespace

auto main() -> int
{
  try
  {
    auto screen = ftxui::Screen::Create(ftxui::Dimension::Fixed(WIDTH),
                                        ftxui::Dimension::Fixed(HEIGHT));

    int alternate = 0;
    for (int row = 0; row < HEIGHT; ++row)
    {
      for (int col = 0; col < WIDTH; ++col)
      {
        auto& cell = screen.PixelAt(col, row);
        if (col == 0)
        {
          cell.character = std::to_string(row / TENS_DIGIT_DIVISOR);
        }
        if (row == 0)
        {
          cell.character = std::to_string(col / TENS_DIGIT_DIVISOR);
        }
        cell.foreground_color = ftxui::Color::White;
        cell.background_color = ftxui::Color::Black;
        if (alternate != 0)
        {
          cell.background_color = ftxui::Color::RGB(0, 0, MAX_RGB_COMPONENT);
        }
        alternate = static_cast<int>(alternate == 0);
      }
      alternate = static_cast<int>(alternate == 0);
    }

    // Access a specific pixel.
    auto& cell = screen.PixelAt(SAMPLE_PIXEL_X, SAMPLE_PIXEL_Y);

    // Set properties of the cell.
    cell.character = "X";
    cell.foreground_color = ftxui::Color::Red;
    cell.background_color = ftxui::Color::RGB(0, MAX_RGB_COMPONENT, 0);
    cell.bold = true; // Set bold style
    screen.Print();   // Print the screen to the terminal

    std::cout << "resolution: width=" << WIDTH << " height=" << HEIGHT << '\n';
  }
  catch (const std::exception& ex)
  {
    std::fputs("error: ", stderr);
    std::fputs(ex.what(), stderr);
    std::fputc('\n', stderr);
    return 1;
  }
  catch (...)
  {
    std::fputs("error: unknown exception\n", stderr);
    return 1;
  }

  return 0;
}
