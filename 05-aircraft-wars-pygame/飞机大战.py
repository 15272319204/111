import pygame
import math
from pygame.constants import *
from config import BG_SIZE
from button import Button

from ms_test.game_scene import start_classic_game
from ms_test.stage_scene import start_stage_game
from ms_test.challenge_scene import start_challenge


class Manager(object):
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode(BG_SIZE, 0, 32)
        pygame.display.set_caption("飞机大战")

        self.in_menu = False
        self.in_diff = False
        self.in_setting = False
        self.in_mode = False
        self.in_stage_select = False
        self.in_challenge_select = False
        self.page_index = 0

        self.difficulty = "普通"
        self.enemy_speed = 10
        self.enemy_spawn_time = 1000
        self.music_on = True
        self.sound_on = True
        self.menu_time = 0

        try:
            self.title_img = pygame.image.load("feiji/name.png").convert_alpha()
        except:
            self.title_img = None
        self.menu_bg = pygame.image.load("feiji/img_bg_level_2.jpg").convert()

    def show_start_splash(self):
        """启动开场：全屏显示飞机大战标题，停留3秒，带淡入淡出效果"""
        start_time = pygame.time.get_ticks()
        wait_ms = 3000
        
        while True:
            now = pygame.time.get_ticks()
            elapsed = now - start_time
            
            alpha = 255
            if elapsed < 500:
                alpha = int(elapsed / 500 * 255)
            elif elapsed > wait_ms - 500:
                alpha = int((wait_ms - elapsed) / 500 * 255)
            
            self.screen.blit(self.menu_bg, (0, 0))
            
            if self.title_img:
                tw, th = self.title_img.get_size()
                x = (BG_SIZE[0] - tw) // 2
                y = (BG_SIZE[1] - th) // 2
                
                scale = 0.8 + (elapsed / 500 * 0.2) if elapsed < 500 else 1.0
                scaled_img = pygame.transform.scale(self.title_img, (int(tw * scale), int(th * scale)))
                scaled_rect = scaled_img.get_rect(center=(BG_SIZE[0] // 2, BG_SIZE[1] // 2))
                
                scaled_img.set_alpha(alpha)
                self.screen.blit(scaled_img, scaled_rect)
            
            pygame.display.update()
            
            if now - start_time >= wait_ms:
                break
            
            for e in pygame.event.get():
                if e.type == QUIT:
                    self.exit_game()

    def exit_game(self):
        pygame.quit()
        exit()

    def drawText(self, text, x, y, size=30, color=(255, 255, 255), shadow_color=(0, 0, 0), shadow_offset=2):
        try:
            font = pygame.font.Font('feiji/baddf.ttf', size)
        except:
            font = pygame.font.SysFont(None, size)
        
        if shadow_color and shadow_offset > 0:
            shadow_surf = font.render(text, True, shadow_color)
            self.screen.blit(shadow_surf, (x + shadow_offset, y + shadow_offset))
        
        surf = font.render(text, True, color)
        self.screen.blit(surf, (x, y))

    def show_main_menu(self):
        bg = self.menu_bg
        self.screen.blit(bg, (0, 0))
        self.menu_time += 1
        
        if self.title_img:
            tw, th = self.title_img.get_size()
            x = (BG_SIZE[0] - tw) // 2
            float_y = 20 + int(math.sin(self.menu_time * 0.05) * 5)
            
            scale = 1.0 + math.sin(self.menu_time * 0.03) * 0.02
            scaled_img = pygame.transform.scale(self.title_img, (int(tw * scale), int(th * scale)))
            scaled_rect = scaled_img.get_rect(center=(BG_SIZE[0] // 2, float_y + th // 2))
            
            self.screen.blit(scaled_img, scaled_rect)

        btn_start = Button(self.screen, "开始游戏", 140, 260, 200, 60, (0, 180, 210), (0, 220, 255))
        btn_diff = Button(self.screen, "难度选择", 140, 340, 200, 60, (100, 180, 100), (140, 220, 140))
        btn_set = Button(self.screen, "设置", 140, 420, 200, 60, (100, 100, 180), (140, 140, 220))
        btn_exit = Button(self.screen, "退出游戏", 140, 500, 200, 60, (180, 60, 60), (220, 80, 80))

        btn_start.draw()
        btn_diff.draw()
        btn_set.draw()
        btn_exit.draw()

        for e in pygame.event.get():
            if e.type == QUIT:
                self.exit_game()
            if btn_start.is_clicked(e):
                self.in_mode = True
                self.in_menu = False
            if btn_diff.is_clicked(e):
                self.in_diff = True
                self.in_menu = False
            if btn_set.is_clicked(e):
                self.in_setting = True
                self.in_menu = False
            if btn_exit.is_clicked(e):
                self.exit_game()
        pygame.display.update()

    def show_mode_select(self):
        bg = self.menu_bg
        self.screen.blit(bg, (0, 0))
        # 顶部常驻标题
        if self.title_img:
            tw, th = self.title_img.get_size()
            x = (BG_SIZE[0] - tw) // 2
            self.screen.blit(self.title_img, (x, 20))
        self.drawText("选择游戏模式", BG_SIZE[0] // 2 - 95, 150, 50, (255, 255, 0))

        btn_classic = Button(self.screen, "经典模式", 120, 280, 240, 55, (0, 170, 170), (0, 210, 210))
        btn_stage = Button(self.screen, "闯关模式", 120, 350, 240, 55, (170, 170, 0), (210, 210, 0))
        btn_challenge = Button(self.screen, "挑战模式", 120, 420, 240, 55, (170, 0, 170), (210, 0, 210))
        btn_back = Button(self.screen, "返回", 120, 500, 240, 55, (100, 100, 100), (140, 140, 140))

        btn_classic.draw()
        btn_stage.draw()
        btn_challenge.draw()
        btn_back.draw()

        for e in pygame.event.get():
            if e.type == QUIT:
                self.exit_game()
            if btn_classic.is_clicked(e):
                start_classic_game(self.screen, self.difficulty, self.enemy_speed, self.enemy_spawn_time, self.music_on,
                                   self.sound_on)
            if btn_stage.is_clicked(e):
                self.in_stage_select = True
                self.in_mode = False
                self.page_index = 0
            if btn_challenge.is_clicked(e):
                self.in_challenge_select = True
                self.in_mode = False
            if btn_back.is_clicked(e):
                self.in_mode = False
                self.in_menu = True
        pygame.display.update()

    def show_stage_select(self):
        bg = self.menu_bg
        self.screen.blit(bg, (0, 0))
        # 顶部常驻标题
        if self.title_img:
            tw, th = self.title_img.get_size()
            x = (BG_SIZE[0] - tw) // 2
            self.screen.blit(self.title_img, (x, 20))
        self.drawText(f"关卡选择 第{self.page_index + 1}/5页", BG_SIZE[0] // 2 - 110, 80, 48, (255, 255, 0))
        start_lv = self.page_index * 20 + 1
        end_lv = start_lv + 19
        if end_lv > 100: end_lv = 100

        btn_list = []
        x_start = 45
        y_start = 160
        gap_x = 85
        gap_y = 70
        per_row = 5
        idx = 0
        for lv in range(start_lv, end_lv + 1):
            row = idx // per_row
            col = idx % per_row
            bx = x_start + col * gap_x
            by = y_start + row * gap_y
            btn = Button(self.screen, f"{lv}关", bx, by, 75, 55, (30, 120, 150), (60, 170, 200))
            btn_list.append([btn, lv])
            btn.draw()
            idx += 1

        btn_prev = Button(self.screen, "上一页", 40, BG_SIZE[1] - 70, 130, 50, (80, 80, 150), (120, 120, 190))
        btn_next = Button(self.screen, "下一页", 200, BG_SIZE[1] - 70, 130, 50, (80, 80, 150), (120, 120, 190))
        btn_backmode = Button(self.screen, "返回模式", 360, BG_SIZE[1] - 70, 150, 50, (150, 50, 50), (190, 80, 80))
        btn_prev.draw()
        btn_next.draw()
        btn_backmode.draw()

        for e in pygame.event.get():
            if e.type == QUIT: self.exit_game()
            for b, lv in btn_list:
                if b.is_clicked(e):
                    start_stage_game(self.screen, lv, self.music_on, self.sound_on)
            if btn_prev.is_clicked(e) and self.page_index > 0:
                self.page_index -= 1
            if btn_next.is_clicked(e) and self.page_index < 4:
                self.page_index += 1
            if btn_backmode.is_clicked(e):
                self.in_stage_select = False
                self.in_mode = True
        pygame.display.update()

    def show_challenge_select(self):
        bg = self.menu_bg
        self.screen.blit(bg, (0, 0))
        # 顶部常驻标题
        if self.title_img:
            tw, th = self.title_img.get_size()
            x = (BG_SIZE[0] - tw) // 2
            self.screen.blit(self.title_img, (x, 20))
        self.drawText("挑战模式", BG_SIZE[0] // 2 - 80, 80, 50, (255, 0, 100))

        levels = [("简单", (0, 180, 0)), ("普通", (0, 180, 180)), ("小难", (0, 100, 200)), ("困难", (180, 180, 0)),
                  ("极难", (200, 100, 0)), ("噩梦", (200, 0, 50)), ("无解", (100, 0, 200))]
        btn_list = []
        y = 160
        for name, color in levels:
            btn = Button(self.screen, name, 120, y, 240, 50, color, (255, 255, 255))
            btn_list.append((btn, name))
            btn.draw()
            y += 58
        btn_back = Button(self.screen, "返回", 120, y + 10, 240, 50, (100, 100, 100), (150, 150, 150))
        btn_back.draw()

        for e in pygame.event.get():
            if e.type == QUIT: self.exit_game()
            for btn, name in btn_list:
                if btn.is_clicked(e):
                    start_challenge(self.screen, name, self.music_on, self.sound_on)
            if btn_back.is_clicked(e):
                self.in_challenge_select = False
                self.in_mode = True
        pygame.display.update()

    def show_diff(self):
        bg = self.menu_bg
        self.screen.blit(bg, (0, 0))
        # 顶部常驻标题
        if self.title_img:
            tw, th = self.title_img.get_size()
            x = (BG_SIZE[0] - tw) // 2
            self.screen.blit(self.title_img, (x, 20))
        self.drawText("全局难度设置", BG_SIZE[0] // 2 - 110, 120, 50, (255, 255, 0))

        diff_list = [
            ("简单", 1, 2500),
            ("普通", 1.8, 1800),
            ("困难", 2.8, 1300)
        ]
        y = 220
        btn_list = []
        for name, speed, spawn in diff_list:
            btn = Button(self.screen, name, 140, y, 200, 55, (20, 120, 150), (50, 170, 200))
            btn.draw()
            btn_list.append((btn, name, speed, spawn))
            y += 70

        btn_back = Button(self.screen, "返回", 140, y + 20, 200, 55, (120, 120, 120), (160, 160, 160))
        btn_back.draw()

        for e in pygame.event.get():
            if e.type == QUIT:
                self.exit_game()
            for btn, name, speed, spawn in btn_list:
                if btn.is_clicked(e):
                    self.difficulty = name
                    self.enemy_speed = speed
                    self.enemy_spawn_time = spawn
            if btn_back.is_clicked(e):
                self.in_diff = False
                self.in_menu = True

        pygame.display.update()

    def show_setting(self):
        bg = self.menu_bg
        self.screen.blit(bg, (0, 0))
        # 顶部常驻标题
        if self.title_img:
            tw, th = self.title_img.get_size()
            x = (BG_SIZE[0] - tw) // 2
            self.screen.blit(self.title_img, (x, 20))
        self.drawText("游戏设置", BG_SIZE[0] // 2 - 70, 130, 50, (255, 255, 0))
        txt1 = "背景音乐:开启" if self.music_on else "背景音乐:关闭"
        txt2 = "音效:开启" if self.sound_on else "音效:关闭"
        btn_music = Button(self.screen, txt1, 130, 230, 220, 55, (40, 130, 170), (70, 170, 210))
        btn_sound = Button(self.screen, txt2, 130, 310, 220, 55, (40, 130, 170), (70, 170, 210))
        btn_back = Button(self.screen, "返回", 130, 390, 220, 55, (130, 130, 130), (170, 170, 170))
        btn_music.draw()
        btn_sound.draw()
        btn_back.draw()

        for e in pygame.event.get():
            if e.type == QUIT: self.exit_game()
            if btn_music.is_clicked(e):
                self.music_on = not self.music_on
            if btn_sound.is_clicked(e):
                self.sound_on = not self.sound_on
            if btn_back.is_clicked(e):
                self.in_setting = False
                self.in_menu = True
        pygame.display.update()

    def run(self):
        # 启动先执行开场3秒画面
        self.show_start_splash()
        # 开场结束进入主菜单
        self.in_menu = True
        while True:
            if self.in_menu:
                self.show_main_menu()
            elif self.in_diff:
                self.show_diff()
            elif self.in_setting:
                self.show_setting()
            elif self.in_mode:
                self.show_mode_select()
            elif self.in_stage_select:
                self.show_stage_select()
            elif self.in_challenge_select:
                self.show_challenge_select()


if __name__ == "__main__":
    game = Manager()
    game.run()