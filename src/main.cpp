#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <windows.h>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <string>

using namespace sf;
using namespace std;

constexpr int blockCount{16};   // количество блоков + 1 пустой
constexpr int blockWidth{128};  // ширина одного блока
constexpr float blockSpeed{16}; // скорость перемещения блока

RenderWindow window(VideoMode(750, 512), "15 puzzle");

int main()
{
    FreeConsole();

    // флаги для запуска игры, остановки, и перемешивания пятнашки
    bool play = false;
    bool stop = true;
    bool shuffle = false;

    // настройки текста
    Font font;
    font.loadFromFile("../res/fonts/Aglettericademicondensedc.otf");
    Text secText("", font, 20);
    secText.setFillColor(Color::Black);
    secText.setPosition(540, 50);

    // параметры для подсчета времени
    clock_t start_t;
    clock_t end_t;

    // пятнашка
    vector<Sprite> tag_sprite; // массив из 16 элементов (15 цифр + 1 пустой слот)
    tag_sprite.resize(blockCount);
    Texture tag_texture;
    tag_texture.loadFromFile("../res/images/15.jpg");
    int grid[4][4]; // сетка 4х4 (будет хранить там текущие номера блоков)
    int *p = &grid[0][0];
    int n = 0;
    for (int i = 0; i < 4; i++) // задаем спрайт каждому блоку
        for (int j = 0; j < 4; j++)
        {
            tag_sprite[n].setTexture(tag_texture);
            tag_sprite[n].setTextureRect(IntRect(i * blockWidth, j * blockWidth, blockWidth, blockWidth));

            grid[i][j] = n; // сохраняем номер блока в массив
            p[n] = n;

            n++;
        }

    Event event;

    // звуки
    SoundBuffer hitBuffer, winBuffer;
    hitBuffer.loadFromFile("../res/sound/score.ogg");
    winBuffer.loadFromFile("../res/sound/win.ogg");
    Sound hitSound(hitBuffer);
    Sound winSound(winBuffer);
    hitSound.setPitch(1.3);
    hitSound.setVolume(40);
    winSound.setVolume(40);

    // изображение того, что нужно собрать
    Sprite example_sprite(tag_texture);
    example_sprite.setScale(0.4, 0.4);
    example_sprite.setPosition(530, 130);

    // настройки кнопки
    Texture button_texture;
    button_texture.loadFromFile("../res/images/button.png");
    Sprite button;
    button.setTexture(button_texture);
    button.setTextureRect(IntRect(0, 130, 196, 65));
    button.setPosition(540, 400);

    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if ((Keyboard::isKeyPressed(Keyboard::Key::Escape)) || (event.type == Event::Closed))
                window.close();

            if (event.type == Event::MouseButtonPressed)
            {
                if (Mouse::isButtonPressed(Mouse::Left))
                {
                    Vector2i pos = Mouse::getPosition(window); // получаем позицию клика ЛКМ

                    if (play) // Если игра запущена
                    {

                        if (pos.x < 512)
                        {
                            int x = pos.x / blockWidth; // вычисляем строку, по которому был совершен клик
                            int y = pos.y / blockWidth; // вычисляем столбец, по которому был совершен клик

                            // воспроизводим звук, если кликнули не по пустому блоку
                            if (grid[x][y] != 15)
                                hitSound.play();

                            int dx = 0; // смещение блока по строке
                            int dy = 0; // смещение блока по столбцу

                            if (grid[x + 1][y] == 15) // если справа кликнутого блока, стоит 15-ый (пустой)
                            {
                                dx = 1; // смещение по строке будет 1 (т.е. вправо)
                                dy = 0; // в столбце не меняется
                            }
                            if (grid[x - 1][y] == 15) // если слева кликнутого блока, стоит 15-ый (пустой)
                            {
                                dx = -1; // смещение по строке будет -1 (т.е. влево)
                                dy = 0;  // в столбце не меняется
                            }
                            if (grid[x][y + 1] == 15) // если сверху кликнутого блока, стоит 15-ый (пустой)
                            {
                                dx = 0; // в строке не меняется
                                dy = 1; // смещение в столбце будет 1 (т.е. вверх)
                            }
                            if (grid[x][y - 1] == 15) // если снизу кликнутого блока, стоит 15-ый (пустой)
                            {
                                dx = 0;  // в строке не меняется
                                dy = -1; // смещение в столбце будет -1 (т.е. вниз)
                            }

                            // меняем блоки местами
                            n = grid[x][y];           // текущий кликнутый сохраняем в n
                            grid[x][y] = 15;          // пустой 15-ый в кликнутый
                            grid[x + dx][y + dy] = n; // на пустой ставим блок n

                            // добавим анимацию перемещения блоков
                            tag_sprite[15].move(-dx * blockWidth, -dy * blockWidth); // перемещаем спрайт пустого блока на кликнутый
                            for (int i = 0; i < blockWidth; i += blockSpeed)
                            {
                                tag_sprite[n].move(blockSpeed * dx, blockSpeed * dy); // перемещаем кликнутый спрайт со скоростью blockSpeed на место пустого
                                window.draw(tag_sprite[15]);                          // рисуем оба блока в цикле
                                window.draw(tag_sprite[n]);
                                window.display();
                            }
                        }
                        if (pos.x >= 540 && pos.x <= 736 && pos.y >= 400 && pos.y <= 465) // Нажатие по кнопке "Начать заново"
                        {
                            shuffle = true;
                            stop = true;
                            play = false;
                        }
                    }
                    else if (stop) // Если игра не началась
                    {
                        if (pos.x >= 540 && pos.x <= 736 && pos.y >= 400 && pos.y <= 465) // Нажатие по кнопке "Старт"
                        {
                            shuffle = true;
                            button.setTextureRect(IntRect(0, 65, 196, 65)); // Меняем текстуру кнопки на "Начать заново"
                        }
                    }
                }
            }
        }
        window.clear(Color::White);

        if (shuffle) // При старте игры необходимо перемешать пятнашку
        {
            random_shuffle(p, p + 4 * 4); // перемешиваем массив, пока он не станет собираемым
            int empty_index, empty_row;
            int sum = 0;
            for (int i = 0; i < blockCount; i++)
            {
                if (p[i] == 15)
                    empty_index = i;
            }
            if (empty_index <= 3)
                empty_row = 1;
            if (empty_index > 3 && empty_index <= 7)
                empty_row = 2;
            if (empty_index > 7 && empty_index <= 11)
                empty_row = 3;
            if (empty_index > 11 && empty_index <= 15)
                empty_row = 4;

            for (int i = 0; i < blockCount; i++)
            {
                int num1 = p[i];
                if (num1 != 15)
                {
                    for (int j = i + 1; j < blockCount; j++)
                    {
                        int num2 = p[j];
                        if (num1 > num2 && num2 != 15)
                            sum += 1;
                    }
                }
            }
            sum += empty_row;
            if (sum % 2 == 0) // начинаем игру
            {
                play = true;
                shuffle = false;
                stop = false;
                start_t = clock();
            }
        }

        for (int i = 0; i < 4; i++) // рисуем каждый блок
            for (int j = 0; j < 4; j++)
            {
                n = grid[i][j];
                tag_sprite[n].setPosition(i * blockWidth, j * blockWidth);
                window.draw(tag_sprite[n]);
            }

        // условие победы
        if (play)
        {
            // таймер
            end_t = clock();
            secText.setString("Time: " + to_string((end_t - start_t) / CLOCKS_PER_SEC) + " s.");

            // проверка на количество правильно расположенных блоков
            int counter = 0;
            n = 0;
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                {
                    if (grid[i][j] == n)
                        counter++;
                    n++;
                }
            // сравнение фактического количества правильно расположенных блоков с нужным
            if (counter == blockCount)
            {
                // заканчиваем игру
                winSound.play();

                end_t = clock();
                secText.setString("Time: " + to_string((end_t - start_t) / CLOCKS_PER_SEC) + " s.");

                stop = true;
                play = false;
                shuffle = false;
                button.setTextureRect(IntRect(0, 130, 196, 65));
            }
        }

        window.draw(secText);
        window.draw(example_sprite);
        window.draw(button);

        window.display();
    }

    return 0;
}