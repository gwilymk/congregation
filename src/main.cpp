#include <SFML/Window.hpp>

int main(int argc, char *argv[])
{
    sf::Window screen(sf::VideoMode(800, 600), "rts");

    screen.display();
}
