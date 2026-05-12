#ifndef GAME_HPP
#define GAME_HPP

void initGame();
void display();
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void specialInput(int key, int x, int y);
void specialInputUp(int key, int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void passiveMouseMotion(int x, int y);
void update(int value);

#endif
