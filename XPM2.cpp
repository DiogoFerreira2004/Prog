#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "XPM2.hpp"
#include "Image.hpp"
#include "Color.hpp"

using namespace std;


namespace prog {

    int hexstrtoInt(const string hexnum) {
        // Função auxiliar que recebe uma string com um número hexadecimal de 8 bits (2 dígitos hexadecimais) e a transforma num integer no formato decimal.
        map<char, int> hex_list = { {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9}, 
                                    {'A', 10}, {'a', 10}, {'B', 11}, {'b', 11 }, {'C', 12}, {'c', 12 }, {'D', 13}, {'d', 13 }, 
                                    {'E', 14}, {'e', 14 },  {'F', 15}, {'f', 15 } };
        int res = 0, len = hexnum.length();
        for (int i = 0; i < len; i++) {
            res = res*16 + hex_list[hexnum[i]];
        }
        return res;
    }

    string colortoHex(const Color color) {
        // Função auxiliar que transforma os valores dos canais de cor (unsigned int) em strings com o valor em formato hexadecimal.
        string res = "#";                              // Cada caracter (tal como indica o formato XPM2), apresenta o scódigo hex. da cor a que está mapeado após um '#'. 
        vector<rgb_value> channels = {color.red(), color.green(), color.blue()};
        map<int, char> hex_list = { {0, '0'}, {1, '1'}, {2, '2'}, {3, '3'}, {4, '4'}, {5, '5'},
                                    {6, '6'}, {7, '7'}, {8, '8'}, {9, '9'}, {10, 'A'},
                                    {11, 'B'}, {12, 'C'}, {13, 'D'}, {14, 'E'}, {15, 'F'} };
        for (rgb_value channel : channels) {           // Channel -> valor do canal de cor que queremos converter.
            if (channel <= 15) {                       // Se channel <= 15, pode ser representado por apenas 1 dígito hexadecimal ...
                res += '0';                            // ... sendo o 1º dígito 0, ...
                res += hex_list[channel];              // ... e o 2º dígito a representação hexadecimal de channel.
            }
            else {                                     // Se channel > 15, a cor não pode ser mapeada por apenas 1 dígito hexadecimal.
                for (int i = 1; i <= 16; i++) {
                    if (channel - i*16 < 0) {          // Quando a expressão for negativa, a representação hexadecimal com i como 1º dígito é maior que o channel (independentemente do 2º dígito) ...
                        channel -= (i-1)*16;           // ... logo, o 1º dígito hexadecimal é o i anterior (maior valor possível menor que o channel) ...
                        res += hex_list[i-1];          // ... sendo, por isso, concatenado à representação.
                        res += hex_list[channel];      // O 2º dígito hexadecimal é o resultado de channel-(i-1)*16, que é sempre <= 15.
                        break;                         // O número está convertido, saindo do loop.
                    }
                }
            }
        }
        return res;
    }

    Image* loadFromXPM2(const string& file) {
        // Função que, a partir de um ficheiro no formato XPM2, cria uma imagem (vetor 2D).
        Image* img = nullptr;                                               // Apontador que conterá a imagem resultante.
        ifstream source_xpm(file);                                          // Permite dar input aos conteúdos de um ficheiro file. 
        char c1, c2, c3, c4, c5, c6, c7, c8, c9, ch_pix;                    // c1-c9: separar os caracteres das linhas do ficheiro XPM que mapeiam as cores; ch_pix: cada um dos caracteres que representa um pixel em formato XMP2.
        int w = 0, h = 0, n = 0, x = 0, y = 0, line_num = 1;                // w: largura da imagem; h: altura da imagem; n: número de cores (mapeadas a caracteres diferentes); (x,y): posição atual do pixel na img; line_num: nº da linha que levará input;
        string line;                                                        // line: string para input das linhas do ficheiro XPM.
        map<char, Color> colors;

        while (getline(source_xpm, line)) {                                 // Percorre todas as linhas do ficheiro até ao seu fim.
            if (line_num == 2) {                                            // A 2ª linha do ficheiro contém sempre as informações w, h, n (c não é usado pois é sempre 1)
                istringstream iss(line);                                    // Permite ler os caracteres da linha atual.
                iss >> w >> h >> n;                                         // Extrair o comprimento, a largura e o número de cores da 2ª linha do ficheiro XPM.
                img = new Image(w, h);
            }
            if (line_num > 2 and line_num <= 2+n) {                         // Após a segunda linha, existem n linhas que mapeiam cada cor a um caracter.
                istringstream iss2(line);
                iss2 >> c1 >> c2 >> c3 >> c4 >> c5 >> c6 >> c7 >> c8 >> c9; // c1: chave; c2: 'c'; c3: '#'; c4,c5: código hex da cor red; c6,c7: ... da cor green; c8,c9: ... da cor blue;

                string str_red(1, c4);                                      // String com código hexadecimal de red (unsigned char, 8 bits, 2 números hexadecimais, sendo c4 o 1º número ...            
                str_red += c5;                                              // ... e c5 o 2º.
                int int_red = hexstrtoInt(str_red);                         // Converter a string de um número hexadecimal num inteiro em base 10.

                string str_green(1, c6);
                str_green += c7;
                int int_green = hexstrtoInt(str_green);

                string str_blue(1, c8);
                str_blue += c9;
                int int_blue = hexstrtoInt(str_blue);

                Color color(int_red, int_green, int_blue);
                colors.insert(make_pair(c1, color));
            }
            if (line_num > 2+n) {                                           // Após as linhas que mapeiam cores a caracteres, existem h linhas com w caracteres que representam essas cores.
                istringstream iss3(line);
                while (iss3 >> ch_pix) {                                    // O loop corre até acabarem todos os caracteres na linha atual.
                    img->at(x,y) = colors[ch_pix];                          // A cor associada ao caracter ch_pix no map colors é atribuída ao pixel na posição (x,y) da imagem.
                    x++;
                    if (x == w) {                                           // Quando acabar a linha (último caracter na posição w-1) ...
                        x = 0;                                              // ... x passa a 0 para começar no 1º elemento da linha seguinte, ...
                        break;                                              // ... terminando a procura de caracteres nessa linha.
                    }
                }
                y++;
                if (y == h) break;                                          // Quando acabarem as linhas (sendo h-1 a última), a imagem em formato XPM2 foi lida, terminando o while da linha 61.
            }
            line_num++;
        }
        return img;
    }

    void saveToXPM2(const string& file, const Image* image) {
        // Função que, a partir de uma imagem (objeto da classe Image), cria um ficheiro no formato XPM2 que a representa.
        map<Color, char> dif_colors;                                        // Conterá todas as cores diferentes e respetivos caracteres para que foram mapeadas.
        int ascii_num = 33, w = image->width(), h = image->height();        // ascii_num: percorre a tabela ASCII, obtendo caracteres para serem mapeados a cores ...
                                                                            // ... (0-32 são caracteres de controlo, que não representam um caracter).
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                map<Color, char>::iterator it;
                it = dif_colors.find(image->at(x,y));
                if (it == dif_colors.end()) dif_colors.insert({ image->at(x,y), ascii_num });   // Se a cor não estiver no map, o iterador aponta para o posição dif_colors.end().
                else continue;                                                                  // Se a cor estiver no map, continua a iteração.
                ascii_num++;
            }
        }
        int n = dif_colors.size();

        ofstream dest_file(file);                                                                                             // Output file stream permite escrever no ficheiro file.xpm.
        dest_file << "! XPM2" << endl << to_string(w) << ' ' << to_string(h) << ' ' << to_string(n) << ' ' << '1' << endl;    // Escrever a 1ª e 2ª linhas no ficheiro XPM.

        for (const auto& color : dif_colors) {
            dest_file << color.second << ' ' << 'c' << ' ' << colortoHex(color.first) << endl;    // Escrever todas as linhas que mapeiam caracteres às respetivas cores.
        }

        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                dest_file << dif_colors[image->at(x,y)];                    // Percorrer todos os pixeis da imagem, escrevendo linhas e colunas de caracteres que mapeiam as respetivas cores.
            }
            dest_file << endl;                                              // De cada vez que acaba uma linha de pixeis na imagem, também acaba uma linha de caracteres no formato XPM2. 
        }
    }
}
