#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>

int main()
{
  using namespace ftxui;

  // Create a styled Hello World DOM element
  Element document =
    hbox({text("Hello"), text(" "), text("World") | bold | color(Color::Blue),
          text("!")}) |
    border;

  // Create a screen to render the document
  auto screen = Screen::Create(Dimension::Full(),       // Width
                               Dimension::Fit(document) // Height
  );
  Render(screen, document);
  screen.Print();

  return 0;
}
