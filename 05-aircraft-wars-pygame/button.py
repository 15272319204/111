import pygame
import math

class Button:
    def __init__(self, screen, text, x, y, width, height, color, hover_color, text_color=(255,255,255)):
        self.screen = screen
        self.text = text
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.color = color
        self.hover_color = hover_color
        self.text_color = text_color
        try:
            self.font = pygame.font.Font('./feiji/baddf.ttf', 30)
        except:
            self.font = pygame.font.SysFont(None, 30)
        self.rect = pygame.Rect(x, y, width, height)
        self.clicked = False
        self.click_animation = 0
        self.shadow_offset = 3

    def draw(self):
        mouse_pos = pygame.mouse.get_pos()
        is_hovered = self.rect.collidepoint(mouse_pos)
        
        scale = 1.0
        if self.click_animation > 0:
            scale = 0.95 + (0.05 * (1.0 - self.click_animation / 10))
            self.click_animation -= 1
        
        current_rect = pygame.Rect(self.x, self.y, self.width, self.height)
        if scale != 1.0:
            current_rect = pygame.Rect(
                self.x + self.width * (1 - scale) / 2,
                self.y + self.height * (1 - scale) / 2,
                self.width * scale,
                self.height * scale
            )
        
        shadow_rect = pygame.Rect(
            current_rect.x + self.shadow_offset,
            current_rect.y + self.shadow_offset,
            current_rect.width,
            current_rect.height
        )
        pygame.draw.rect(self.screen, (0, 0, 0, 100), shadow_rect, border_radius=8)
        
        current_color = self.hover_color if is_hovered else self.color
        
        pygame.draw.rect(self.screen, current_color, current_rect, border_radius=8)
        
        border_color = (min(255, current_color[0] + 50), min(255, current_color[1] + 50), min(255, current_color[2] + 50))
        pygame.draw.rect(self.screen, border_color, current_rect, border_radius=8, width=2)
        
        text_surf = self.font.render(self.text, True, self.text_color)
        text_rect = text_surf.get_rect(center=current_rect.center)
        self.screen.blit(text_surf, text_rect)

    def is_clicked(self, event):
        if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
            if self.rect.collidepoint(event.pos):
                self.click_animation = 10
                return True
        return False