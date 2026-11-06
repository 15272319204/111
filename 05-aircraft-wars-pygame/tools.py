import pygame
from config import BG_SIZE

class GameSound(object):
    def __init__(self):
        pygame.mixer.init()
        pygame.mixer.music.load("./feiji/bg2.ogg")
        pygame.mixer.music.set_volume(0.5)
        self.__bomb = pygame.mixer.Sound("./feiji/bomb.wav")

    def playBackgroundMusic(self):
        pygame.mixer.music.play(-1)

    def playBombSound(self):
        pygame.mixer.Sound.play(self.__bomb)

class Bomb(object):
    def __init__(self, screen, type):
        self.screen = screen
        if type == 'enemy':
            self.mImages = [pygame.image.load("./feiji/enemy0_down"+str(v)+".png") for v in range(1,5)]
        else:
            self.mImages = [pygame.image.load("./feiji/hero_destroy"+str(v)+".png") for v in range(1,4)]
        self.mImages += self.mImages
        self.mIndex = 0
        self.mPos = [0,0]
        self.mVisible = False

    def action(self, rect):
        self.mPos[0] = rect.left
        self.mPos[1] = rect.top
        self.mVisible = True

    def draw(self):
        if not self.mVisible:
            return
        self.screen.blit(self.mImages[self.mIndex], (self.mPos[0], self.mPos[1]))
        self.mIndex +=1
        if self.mIndex >= len(self.mImages):
            self.mIndex =0
            self.mVisible = False

class GameBackground(object):
    def __init__(self, screen):
        self.mImage1 = pygame.image.load("./feiji/img_bg_level_2.jpg")
        self.mImage2 = pygame.image.load("./feiji/img_bg_level_2.jpg")
        self.screen = screen
        self.y1 = 0
        self.y2 = -BG_SIZE[1]

    def move(self):
        self.y1 +=2
        self.y2 +=2
        if self.y1 >= BG_SIZE[1]:
            self.y1 =0
        if self.y2 >=0:
            self.y2 = -BG_SIZE[1]

    def draw(self):
        self.screen.blit(self.mImage1, (0, self.y1))
        self.screen.blit(self.mImage2, (0, self.y2))