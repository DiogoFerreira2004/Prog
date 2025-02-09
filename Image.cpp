#include "Image.hpp"
#include <vector>


namespace prog
{
    Image::Image(int w, int h, const Color &fill)
    { // Criar imagem (vetor 2D) com comprimento w e altura h, em que cada píxel (elemento do vetor 2D) tem a cor fill (por omissão é a cor branca).    
        width_ = w;
        height_ = h;
        std::vector<std::vector<Color>> img(height_, std::vector<Color>(width_, fill));
        colors_ = img;

    }
    Image::~Image() // Não é necessário, pois não alocamos o vetor 2D em memória dinâmica.
    {
    }


    int Image::width() const // Função que retorna o comprimento da imagem.
    {
        return width_;
    }
    int Image::height() const // Função que retorna a altura da imagem.
    {
        return height_;
    }


    Color& Image::at(int x, int y) // Função que retorna uma referência mutável à cor do pixel de coordendas (x, y).
    {
        return colors_[y][x];      // O índice do 1º operador [] de um vetor 2D é número da linha, representado pelo y.
    }

    const Color& Image::at(int x, int y) const // Função que retorna a cor (formada pelas suas três variáveis red, green e blue) do pixel de coordenadas (x, y).
    {
        return colors_[y][x];
    }
}
