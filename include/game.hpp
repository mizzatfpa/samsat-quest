#ifndef GAME_HPP
#define GAME_HPP

void initGame();
void display();
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void specialInput(int key, int x, int y);
void update(int value);

#endif
