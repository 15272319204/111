import pygame
from pygame.constants import *

class HeroPlane(pygame.sprite.Sprite):
    bullets = pygame.sprite.Group()

    def __init__(self, screen):
        pygame.sprite.Sprite.__init__(self)
        self.image = pygame.image.load('./feiji/hero1.png')
        self.rect = self.image.get_rect()
        self.screen = screen
        self.screen_rect = screen.get_rect()
        self.rect.centerx = self.screen_rect.centerx
        self.rect.bottom = self.screen_rect.bottom
        self.speed = 15
        self.bullets = pygame.sprite.Group()
        self.hp = 5  # 血量
        self.speed_multiplier = 1

    def key_control(self):
        key_pressed = pygame.key.get_pressed()
        current_speed = self.speed * self.speed_multiplier
        if (key_pressed[K_w] or key_pressed[K_UP]) and self.rect.top > self.screen_rect.top:
            self.rect.top -= current_speed
        if (key_pressed[K_s] or key_pressed[K_DOWN]) and self.rect.bottom < self.screen_rect.bottom:
            self.rect.bottom += current_speed
        if (key_pressed[K_a] or key_pressed[K_LEFT]) and self.rect.left > 0:
            self.rect.left -= current_speed
        if (key_pressed[K_d] or key_pressed[K_RIGHT]) and self.rect.right < self.screen_rect.right:
            self.rect.right += current_speed
        if key_pressed[K_SPACE]:
            bullet = Bullet(self.screen, self.rect.left, self.rect.top)
            self.bullets.add(bullet)
            HeroPlane.bullets.add(bullet)

    def update(self):
        self.key_control()
        self.display()

    def display(self):
        self.screen.blit(self.image, self.rect)
        self.bullets.update()
        self.bullets.draw(self.screen)

    @classmethod
    def clear_bullets(cls):
        cls.bullets.empty()

class Bullet(pygame.sprite.Sprite):
    def __init__(self, screen, x, y):
        pygame.sprite.Sprite.__init__(self)
        self.image = pygame.image.load('./feiji/bullet.png')
        self.rect = self.image.get_rect()
        self.rect.topleft = [x + 100 / 2 - 22 / 2, y - 25]
        self.screen = screen
        self.speed = 20

    def update(self):
        self.rect.top -= self.speed
        if self.rect.top < -22:
            self.kill()