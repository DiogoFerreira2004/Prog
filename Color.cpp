#include "Color.hpp"


namespace prog {
    Color::Color() {
        // Construtor com a cor preta (valor 0 em cada uma das variáveis-membro red_, green_ e blue_).
        red_ = 0;
        green_ = 0;
        blue_ = 0;
    }
    Color::Color(const Color& other) {
        // Construtor que cria um objeto com a mesma cor do argumento "other".
        red_ = other.red_;
        green_ = other.green_;
        blue_ = other.blue_;
    }
    Color::Color(rgb_value red, rgb_value green, rgb_value blue) {
        // Construtor que cria um objeto com a cor determinada pelos valores dos respetivos argumentos.
        red_ = red;
        green_ = green;
        blue_ = blue;
    }

    // Funções que retornam o valor de cada um dos respetivos canais de cor.
    rgb_value Color::red() const {
        return red_;
    }
    rgb_value Color::green() const {
        return green_;
    }
    rgb_value Color::blue() const {
        return blue_;
    }

    // Funções que retornam uma referência (modificável) do valor de cada um dos respetivos canais de cor.
    rgb_value& Color::red()  {
        return red_;
    }
    rgb_value& Color::green()  {
        return green_;
    }
    rgb_value& Color::blue()  {
        return blue_;
    }

    bool Color::operator<(const Color& color2) const {
        // Função que define o operador '<' entre dois objetos da class Color (operação usada na função comparadora do container map).
        if (this->red() < color2.red()) return true;
        if (this->red() == color2.red() and this->green() < color2.green()) return true;
        if (this->red() == color2.red() and this->green() == color2.green() and this->blue() < color2.blue()) return true;
        return false;
    }
}
