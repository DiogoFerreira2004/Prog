#include <iostream>
#include <fstream>
#include <algorithm>
#include "Script.hpp"
#include "PNG.hpp"
#include "XPM2.hpp"

using namespace std;


namespace prog {
    // Use to read color values from a script file.
    istream& operator>>(istream& input, Color& c) {
        int r, g, b;
        input >> r >> g >> b;
        c.red() = r;
        c.green() = g;
        c.blue() = b;
        return input;
    }

    Script::Script(const string& filename) :
            image(nullptr), input(filename) {

    }
    void Script::clear_image_if_any() {
        if (image != nullptr) {
            delete image;
            image = nullptr;
        }
    }
    Script::~Script() {
        clear_image_if_any();
    }

    void Script::run() {
        string command;
        while (input >> command) {
            cout << "Executing command '" << command << "' ..." << endl;
            if (command == "open") {
                open();
                continue;
            }
            if (command == "blank") {
                blank();
                continue;
            }
            // Other commands require an image to be previously loaded.
            if (command == "save") {
                save();
                continue;
            }

            if (command == "invert") {
                invert();
                continue;
            }
            if (command == "to_gray_scale") {
                to_gray_scale();
                continue;
            }
            if (command == "replace") {
                replace();
                continue;
            }
            if (command == "fill") {
                fill();
                continue;
            }
            if (command == "h_mirror") {
                h_mirror();
                continue;
            }
            if (command == "v_mirror") {
                v_mirror();
                continue;
            }
            if (command == "add") {
                add();
                continue;
            }
            if (command == "crop") {
                crop();
                continue;
            }
            if (command == "rotate_left") {
                rotate_left();
                continue;
            }
            if (command == "rotate_right") {
                rotate_right();
                continue;
            }
            if (command == "median_filter") {
                median_filter();
                continue;
            }
            if (command == "xpm2_open") {
                xpm2_open();
                continue;
            }
            if (command == "xpm2_save") {
                xpm2_save();
                continue;
            }
        }
    }
    void Script::open() {
        // Replace current image (if any) with image read from PNG file.
        clear_image_if_any();
        string filename;
        input >> filename;
        image = loadFromPNG(filename);
    }
    void Script::blank() {
        // Replace current image (if any) with blank image.
        clear_image_if_any();
        int w, h;
        Color fill;
        input >> w >> h >> fill;
        image = new Image(w, h, fill);
    }
    void Script::save() {
        // Save current image to PNG file.
        string filename;
        input >> filename;
        saveToPNG(filename, image);
    }

    void Script::invert() {
        // Transformar os canais de cor de cada pixel (objeto da class Color com canais de cor red_, green_, blue_) no seu inverso.
        for (int y = 0; y < image->height(); y++){         // Aceder a cada um dos pixeis da imagem (Vetor 2D) com um nested loop ...
            for (int x = 0; x < image->width(); x++){      // ... tal como acontecerá na maior parte das seguintes funções.
                Color& pix = image->at(x, y);              // Passar o pixel que será invertido por referência (de modo a poderem ser efetuadas alterações ao pixel).
                pix.red() = 255 - pix.red();               // Computar o inverso de cada um dos canais de cor.
                pix.green() = 255 - pix.green();
                pix.blue() = 255 - pix.blue();
            }
        }
    }

    void Script::to_gray_scale() {
        // Transformar imagem com cores para tons de cinza, segundo o fator v.
        rgb_value v;

        for (int y = 0; y < image->height(); y++){                  // Função height() definida em Image.cpp retorna a altura da imagem.
            for (int x = 0; x < image->width(); x++){               // Função width() definida em Image.cpp retorna a largura da imagem.
                Color& pix = image->at(x, y);
                v = (pix.red() + pix.green() + pix.blue()) / 3;     // O fator v depende dos valores dos canais de cor (red_, green_, blue_) de cada pixel.
                pix.red() = v;                                      // Cada um dos canais de cor de cada pixel toma o valor de v.
                pix.green() = v;
                pix.blue() = v;
            }
        }
    }

    void Script::replace() {
        // Substituir pixeis de uma imagem, de uma cor por outra (são fornecidas à função ambas as cores).
        int r1, g1, b1, r2, g2, b2;
        input >> r1 >> g1 >> b1 >> r2 >> g2 >> b2;                                   // A file input stream "input" (como definida em Script.hpp) lê os argumentos fornecidos no script.

        for (int y = 0; y < image->height(); y++){
            for (int x = 0; x < image->width(); x++){
                Color& pix = image -> at(x,y);
                if (pix.red() == r1 and pix.green() == g1 and pix.blue() == b1){     // Se o pixel tem as cores (r1, g1, b1) ...
                    pix.red() = r2;                                                  // ... substituí-las por (r2, g2, b2).
                    pix.green() = g2;
                    pix.blue() = b2;
                }
            }
        }
    }

    void Script::fill() {
        // Substituir as cores dos pixeis dentro de um retângulo com largura w, altura h e canto superior esquerdo de coordenadas (x, y).
        int x, y, w, h, r, g, b;
        input >> x >> y >> w >> h >> r >> g >> b;

        for (int i = x; i < (x+w); i++){          // Percorrer os pixeis do retângulo contido na imagem com um nested loop.
            for (int j = y; j < (y+h); j++){
                Color& pix = image->at(i,j);
                pix.red() = r;                    // Substituir a cor de cada um desses pixeis pela cor (r, g, b).
                pix.green() = g;
                pix.blue() = b;
            }
        }
    }

    void Script::h_mirror() {
        // Espelhar imagem horizontalmente (de acordo com eixo vertical na posição x == largura_da_imagem / 2).
        for (int y = 0; y < image->height(); y++){                      // Percorrer os pixeis da imagem ...
            for (int x = 0; x < image->width() / 2; x++){               // ... até metade da largura.
                Color& pix_esq = image->at(x,y);                        // Passar o pixel da esquerda por referência.
                Color& pix_dir = image->at((image->width()-1) - x, y);  // Passar o pixel da direita (pela simetria segundo o eixo vertical) por referência.
                swap(pix_esq.red(), pix_dir.red());                     // Trocar cada um dos canais de cor entre os dois pixeis.
                swap(pix_esq.green(), pix_dir.green());
                swap(pix_esq.blue(), pix_dir.blue());
            }
        }
    }

    void Script::v_mirror() {
        // Espelhar imagem verticalmente (de acordo com eixo horizontal na posição y == altura_da_imagem / 2).
        for (int y = 0; y < image->height() / 2; y++){                      // Percorrer os pixeis da imagem até metade da altura.
            for (int x = 0; x < image->width(); x++){
                Color& pix_cima = image->at(x,y);                           // Passar o pixel de cima por referência.
                Color& pix_baixo = image->at(x, (image->height()-1) - y);   // Passar o pixel de baixo (pela simetria segundo o eixo horizontal) por referência.
                swap(pix_cima.red(), pix_baixo.red());
                swap(pix_cima.green(), pix_baixo.green());
                swap(pix_cima.blue(), pix_baixo.blue());
            }
        }
    }



    void Script::add() {
        // Copiar todos os pixeis da imagem dada (no ficheiro filename) para a imagem atual (a partir do canto superior esquerdo (x, y)), de acordo com uma condição.
        string filename;
        int r, g, b, x, y;
        input >> filename >> r >> g >> b >> x >> y;
        Image* image_to_copy = loadFromPNG(filename);                   // Apontador que se refere à imagem de onde se pretende copiar pixeis.

        for (int line = 0; line < image_to_copy->height(); line++){     // Imagem do ficheiro é sempre menor que a imagem atual.
            for (int col = 0; col < image_to_copy->width(); col++){
                Color& pix_to_copy = image_to_copy->at(col,line);
                if (!(pix_to_copy.red() == r and                        // Condição: se o pixel a ser copiado não for de cor (r, g, b), é copiado para a imagem atual.
                      pix_to_copy.green() == g and
                      pix_to_copy.blue() == b)) {
                    Color& res_pix = image->at(x+col,y+line);           // A posição do pixel da imagem atual (image) que será alterado corresponde ...
                    res_pix.red() = pix_to_copy.red();                  // ... à posição na imagem dada (de modo a não alterar a posição relativa entre pixeis) ...
                    res_pix.green() = pix_to_copy.green();              // ... adicionando o respetivo offset (x, y) a partir do qual a imagem dada será posicionada na imagem atual.
                    res_pix.blue() = pix_to_copy.blue();
                }
            }
        }
        delete image_to_copy;                                           // Libertar a memória alocada ao criar o pointer image_to_copy.
        image_to_copy = NULL;                                           // Evitar dangling pointers.
    }

    void Script::crop() {
        // Recortar a imagem atual a partir da posição (x,y), no retângulo de largura w e altura h.
        int x, y, w, h;
        input >> x >> y >> w >> h;
        Image res_image(w,h);                                           // Conterá a imagem recortada.

        for (int res_y = 0; res_y < h; res_y++){                        // Percorrer os pixeis da res_image com um nested loop.
            for (int res_x = 0; res_x < w; res_x++){
                Color& res_pix = res_image.at(res_x, res_y);
                Color& pix_to_copy = image->at(x + res_x, y + res_y);   // Pixel que será copiado (da imagem atual, image) -> posição inicial (x,y) + atual posição do pixel do res_image ...
                res_pix.red() = pix_to_copy.red();                      // ... (de modo a não alterar a posição relativa dos pixeis).
                res_pix.green() = pix_to_copy.green();
                res_pix.blue() = pix_to_copy.blue();
            }
        } 
        *image = res_image;                                             // Sendo image o apontador que se refere à imagem que resultará da função (como definido em Script.hpp) ... 
    }                                                                   // ... modifica-se o seu conteúdo da imagem atual para a imagem cortada (res_image).

    void Script::rotate_left() {
        // Rodar uma imagem 90° para a esquerda.
        Image rotated_img(image->height(), image->width());                 // Imagem que recebe os pixeis rodados.

        for (int y = 0; y < image->height(); y++){                          // Percorrer os pixeis da imagem não rodada.
            for (int x = 0; x < image->width(); x++){
                rotated_img.at(y, (image->width()-1) - x) = image->at(x,y); // A posição de cada pixel rodado relaciona-se com a posição original segundo a relação: ... 
            }                                                               // ... a coordenada horizontal é y; a vertical é o número de elementos numa linha na imagem original-1-x
        }
        *image = rotated_img;                                               // Tal como na função anterior, modifica-se o conteúdo do apontador para a imagem rodada. 
    }

    void Script::rotate_right() {
        // Rodar uma imagem 90° para a direita.
        Image rotated_img(image->height(), image->width());                     // Ao rodar uma imagem 90°, a sua largura passa a ser a altura inicial e ...
                                                                                // ... a altura torna-se na sua largura inicial.
        for (int y = 0; y < image->height(); y++){
            for (int x = 0; x < image->width(); x++){
                rotated_img.at((image->height()-1) - y, x) = image->at(x,y);    // Neste caso, a coordenada horizontal é o número de linhas da imagem original-1-y, e a vertical é x.
            }
        }
        *image = rotated_img;
    }

    void Script::median_filter(){
        // Substituir cada canal de cor de cada pixel, pela mediana dos canais de cor dos pixeis numa visinhança em torno desse pixel, de lado ws.
        int ws, width = image->width(), height = image->height();   // ws é a dimensão do lado da vizinhança (quadrada) do pixel.
        Image median_img(width, height);                            // Imagem com as mesmas dimensões da imagem atual. Conterá a imagem resultante.
        vector<int> red_viz, green_viz, blue_viz;                   // 3 vetores com os valores dos canais de cor red, green e blue, da vizinhança do pixel analisado.
        size_t num_viz;                                             // Conterá o tamanho dos vetores red_viz, blue_viz, green_viz (tamanhos iguais), ou seja, o número de vizinhos do pixel analisado.
        input >> ws;

        for (int y = 0; y < height; y++){
            for (int x = 0; x < width; x++){
                for (int ny = max(0, y - ws / 2); ny <= min(height - 1, y + ws / 2); ny++){     // Para cada pixel, percorrer a sua vizinhança (com um nested loop) de acordo com as funções max e min que ...
                    for (int nx = max(0, x - ws / 2); nx <= min(width - 1, x + ws / 2); nx++){  // ... averiguam os casos em que o número de vizinhos do pixel é menor que ws*ws (perto das bordas do vector 2D).
                        Color& pix_viz = image->at(nx, ny);                                     // Passar cada pixel vizinho por referência.
                        red_viz.push_back(pix_viz.red());                                       // Guardar cada um dos canais de cor desse pixel no respetivo vetor.
                        green_viz.push_back(pix_viz.green());
                        blue_viz.push_back(pix_viz.blue());
                    }
                }
                sort(red_viz.begin(), red_viz.end());                                           // Ordenar cada um dos vetores para computar a mediana.
                sort(blue_viz.begin(), blue_viz.end());
                sort(green_viz.begin(), green_viz.end());
                num_viz = red_viz.size();                                                       // Obter o número de vizinhos para computar a mediana.

                Color& res_pix = median_img.at(x, y);                                           // Passar o pixel da imagem que conterá o resultado por referência.

                if (num_viz % 2 != 0) {                                                         // Computar a mediana. No caso do número de vizinhos ser ímpar, é o elemento central ...
                    res_pix.red() = red_viz[num_viz/2];
                    res_pix.green() = green_viz[num_viz/2];
                    res_pix.blue() = blue_viz[num_viz/2];
                }
                else {
                    res_pix.red() = (red_viz[num_viz/2 - 1] + red_viz[num_viz/2])/2;            // ... se o número de vizinhos é par, a mediana é a média dos elementos centrais.
                    res_pix.green() = (green_viz[num_viz/2 - 1] + green_viz[num_viz/2])/2;
                    res_pix.blue() = (blue_viz[num_viz/2 - 1] + blue_viz[num_viz/2])/2;
                }

                red_viz.clear();                                                                // Esvaziar os vetores para a próxima iteração do loop.
                green_viz.clear();
                blue_viz.clear();
            }
        }
        *image = median_img;
    }

    void Script::xpm2_open() {
        // Função semelhante à função open() fornecida, que em vez de criar um objeto da classe Image a partir de um ficheiro PNG, cria a partir de um ficheiro em formato XPM2.
        clear_image_if_any();
        string filename;
        input >> filename;
        image = loadFromXPM2(filename);
    }

    void Script::xpm2_save() {
        // Função semelhante à função save() fornecida, que em vez de transformar um objeto da classe Image num ficheiro PNG, transforma-o num ficheiro em formato XPM2.
        string filename;
        input >> filename;
        saveToXPM2(filename, image);
    }
}
