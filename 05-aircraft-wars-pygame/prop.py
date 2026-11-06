import pygame
import random

# 道具组
props = pygame.sprite.Group()

class Prop(pygame.sprite.Sprite):
    def __init__(self, screen, x=None, y=None, prop_type=None):
        super().__init__()
        self.screen = screen
        self.prop_type = prop_type if prop_type is not None else random.choice(['shield', 'speed', 'score', 'bomb'])
        self.x = x if x is not None else random.randint(20, pygame.display.get_surface().get_width()-60)
        self.y = y if y is not None else -50
        self.width = 30
        self.height = 30
        self.rect = pygame.Rect(self.x - self.width // 2, self.y - self.height // 2, self.width, self.height)
        self.speed = 2
        self.damage = 10
        self.load_images()

    def load_images(self):
        colors = {
            'shield': (0, 255, 255),    # 青色护盾
            'speed': (255, 165, 0),     # 橙色加速
            'score': (255, 215, 0),     # 金色双分
            'bomb': (255, 255, 0)       # 黄色炸弹
        }
        self.color = colors.get(self.prop_type, (255, 255, 255))
        self.image = pygame.Surface((self.width, self.height), pygame.SRCALPHA)
        pygame.draw.circle(self.image, self.color, (self.width // 2, self.height // 2), self.width // 2)
        pygame.draw.circle(self.image, (255, 255, 255), (self.width // 2, self.height // 2), self.width // 2, 2)

    def update(self):
        self.rect.y += self.speed
        if self.rect.top > pygame.display.get_surface().get_height():
            self.kill()

    def draw(self):
        self.screen.blit(self.image, self.rect)
