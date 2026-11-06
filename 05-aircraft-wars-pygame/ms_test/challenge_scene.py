import pygame
import random
import time
import math
import sys
import os
from pygame.constants import *
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from config import BG_SIZE, CREATE_ENEMY_ID
from player import HeroPlane
from enemy import boss_bullets
from tools import GameBackground, Bomb, GameSound
from button import Button

WIDTH, HEIGHT = BG_SIZE

class Prop(pygame.sprite.Sprite):
    def __init__(self,screen,prop_type):
        super().__init__()
        self.screen = screen
        self.prop_type = prop_type
        self.damage = 2 if prop_type==0 else 3
        try:
            self.image = pygame.image.load(f"./feiji/prop_type_{prop_type}.png").convert_alpha()
            self.image = pygame.transform.scale(self.image, (40, 40))
        except:
            self.image = pygame.Surface((40,40))
            self.image.fill((255,255,0) if prop_type==0 else (255,100,0))
        self.rect = self.image.get_rect()
        self.rect.x = random.randint(20,WIDTH-60)
        self.rect.y = -50
        self.speed = 2
    def update(self):
        self.rect.y += self.speed
        if self.rect.y > HEIGHT+20:
            self.kill()

class ChallengeScene:
    def __init__(self, screen, difficulty_name, music_on, sound_on):
        self.screen = screen
        self.difficulty_name = difficulty_name
        self.music_on = music_on
        self.sound_on = sound_on
        self.config = self.get_config()
        self.boss_max = self.config["boss_count"]
        self.boss_hp_base = self.config["boss_hp"]
        self.boss_fire_rate = self.config["fire_rate"]
        self.bullet_speed = self.config["bullet_speed"]
        self.bullet_count = self.config["bullet_count"]

        self.bg_list = []
        try:
            img = pygame.image.load("./feiji/img_bg_level_5.jpg").convert()
            img = pygame.transform.scale(img, BG_SIZE)
            self.bg_list.append(img)
            self.bg_list.append(img)
        except:
            self.bg_list.append(pygame.Surface(BG_SIZE))
            self.bg_list.append(pygame.Surface(BG_SIZE))

        self.bg_index = 0
        self.current_bg = self.bg_list[0]
        self.next_bg = self.bg_list[1]
        self.bg_scroll_y = -HEIGHT

        try:
            self.gameover_img = pygame.image.load("./feiji/gameover2.png").convert_alpha()
            self.gameover_img = pygame.transform.scale(self.gameover_img, BG_SIZE)
        except:
            self.gameover_img = pygame.Surface(BG_SIZE)
            self.gameover_img.fill((150,30,30))
        self.show_gameover_img = False
        self.gameover_start_time = 0
        self.gameover_hold_ms = 1000

        self.prop_group = pygame.sprite.Group()
        self.prop_tick = 0

        self.map = GameBackground(screen)
        self.players = pygame.sprite.Group()
        self.boss = None
        self.boss_killed = 0
        self.game_win = False
        self.player_bomb = Bomb(screen, 'player')
        self.enemy_bomb = Bomb(screen, 'enemy')
        self.sound = GameSound()
        self.is_paused = False
        self.is_game_over = False
        self.running = True
        try:
            self.ui_font = pygame.font.Font('./feiji/baddf.ttf', 20)
        except:
            self.ui_font = pygame.font.SysFont(None, 20)

    def get_config(self):
        return {
            "简单": {"boss_count":1,"boss_hp":30,"fire_rate":100,"bullet_speed":6,"bullet_count":8},
            "普通": {"boss_count":2,"boss_hp":50,"fire_rate":80,"bullet_speed":7,"bullet_count":12},
            "小难": {"boss_count":2,"boss_hp":80,"fire_rate":65,"bullet_speed":8,"bullet_count":16},
            "困难": {"boss_count":3,"boss_hp":120,"fire_rate":50,"bullet_speed":9,"bullet_count":20},
            "极难": {"boss_count":3,"boss_hp":180,"fire_rate":40,"bullet_speed":10,"bullet_count":24},
            "噩梦": {"boss_count":4,"boss_hp":260,"fire_rate":30,"bullet_speed":11,"bullet_count":30},
            "无解": {"boss_count":4,"boss_hp":400,"fire_rate":20,"bullet_speed":13,"bullet_count":40},
        }[self.difficulty_name]

    def reset(self):
        boss_bullets.empty()
        HeroPlane.clear_bullets()
        self.players.empty()
        self.boss = None
        self.boss_killed = 0
        self.game_win = False
        self.is_paused = False
        self.is_game_over = False
        self.bg_index = 0
        self.current_bg = self.bg_list[0]
        self.next_bg = self.bg_list[1]
        self.bg_scroll_y = -HEIGHT
        self.show_gameover_img = False
        self.gameover_start_time = 0
        self.prop_group.empty()
        self.prop_tick = 0
        self.players.add(HeroPlane(self.screen))
        self.spawn_new_boss()
        if self.music_on: self.sound.playBackgroundMusic()

    def spawn_new_boss(self):
        self.boss = ChallengeBoss(self.screen, hp=self.boss_hp_base, fire_rate=self.boss_fire_rate, bullet_speed=self.bullet_speed, bullet_count=self.bullet_count)

    def drawText(self, text, x, y, size=30, color=(255,255,255)):
        try: font = pygame.font.Font('./feiji/baddf.ttf', size)
        except: font = pygame.font.SysFont(None, size)
        
        shadow_surf = font.render(text, True, (0, 0, 0))
        self.screen.blit(shadow_surf, (x + 2, y + 2))
        
        self.screen.blit(font.render(text, True, color), (x,y))

    def draw_boss_health_bar(self):
        if not self.boss:
            return
        
        bar_width = 300
        bar_height = 20
        bar_x = (WIDTH - bar_width) // 2
        bar_y = 50
        
        bg_rect = pygame.Rect(bar_x, bar_y, bar_width, bar_height)
        pygame.draw.rect(self.screen, (50, 50, 50), bg_rect, border_radius=10)
        
        hp_percent = max(0, self.boss.hp / self.boss.max_hp)
        fill_width = int(bar_width * hp_percent)
        
        gradient_color = (255 - int(255 * hp_percent), int(255 * hp_percent), 50)
        fill_rect = pygame.Rect(bar_x, bar_y, fill_width, bar_height)
        pygame.draw.rect(self.screen, gradient_color, fill_rect, border_radius=10)
        
        border_rect = pygame.Rect(bar_x, bar_y, bar_width, bar_height)
        pygame.draw.rect(self.screen, (255, 255, 255), border_rect, border_radius=10, width=2)
        
        hp_text = f"Boss HP: {self.boss.hp}/{self.boss.max_hp}"
        text_surf = self.ui_font.render(hp_text, True, (255, 255, 255))
        text_rect = text_surf.get_rect(center=(WIDTH // 2, bar_y + bar_height // 2))
        self.screen.blit(text_surf, text_rect)

    def show_pause(self):
        s = pygame.Surface(BG_SIZE)
        s.set_alpha(190)
        s.fill((0,0,0))
        self.screen.blit(s, (0,0))
        self.drawText("挑战暂停", WIDTH//2-90,160,55,(255,255,0))
        btn_c = Button(self.screen, "继续",130,280,220,55,(0,170,170),(0,210,210))
        btn_r = Button(self.screen, "重新开始",130,350,220,55,(0,170,0),(0,210,0))
        btn_b = Button(self.screen, "返回",130,420,220,55,(180,0,0),(220,0,0))
        btn_c.draw()
        btn_r.draw()
        btn_b.draw()
        for e in pygame.event.get():
            if e.type == QUIT: exit()
            if btn_c.is_clicked(e): self.is_paused=False
            if btn_r.is_clicked(e): self.reset()
            if btn_b.is_clicked(e): self.running=False
        pygame.display.update()

    def show_gameover(self):
        s = pygame.Surface(BG_SIZE)
        s.set_alpha(200)
        s.fill((0,0,0))
        self.screen.blit(s, (0,0))
        self.drawText("挑战失败", WIDTH//2-90,200,55,(255,0,0))
        btn_r = Button(self.screen, "重新挑战",120,350,240,55,(0,170,0),(0,210,0))
        btn_b = Button(self.screen, "返回",120,430,240,55,(180,0,0),(220,0,0))
        btn_r.draw()
        btn_b.draw()
        for e in pygame.event.get():
            if e.type == QUIT: exit()
            if btn_r.is_clicked(e): self.reset()
            if btn_b.is_clicked(e): self.running=False
        pygame.display.update()

    def show_win(self):
        s = pygame.Surface(BG_SIZE)
        s.set_alpha(200)
        s.fill((0,0,0))
        self.screen.blit(s, (0,0))
        self.drawText("挑战成功！", WIDTH//2-100,200,55,(0,255,0))
        btn_b = Button(self.screen, "返回",120,350,240,55,(180,0,0),(220,0,0))
        btn_b.draw()
        for e in pygame.event.get():
            if e.type == QUIT: exit()
            if btn_b.is_clicked(e): self.running=False
        pygame.display.update()

    def run(self):
        self.reset()
        clock = pygame.time.Clock()
        while self.running:
            dt = clock.tick(60)
            if self.is_paused:
                self.show_pause()
                continue
            if self.show_gameover_img:
                self.screen.blit(self.gameover_img, (0,0))
                if pygame.time.get_ticks() - self.gameover_start_time >= self.gameover_hold_ms:
                    self.show_gameover_img = False
                    self.is_game_over = True
                pygame.display.update()
                continue
            if self.is_game_over:
                self.show_gameover()
                continue
            if self.game_win:
                self.show_win()
                continue

            gap = max(70,220-self.boss_killed*35)
            self.prop_tick +=1
            if self.prop_tick >= gap:
                self.prop_group.add(Prop(self.screen,0))
                self.prop_tick = 0
            if self.prop_tick%int(gap*1.9)==0:
                self.prop_group.add(Prop(self.screen,1))

            self.screen.blit(self.current_bg, (0,0))
            self.screen.blit(self.next_bg, (0, self.bg_scroll_y))
            self.bg_scroll_y += 2
            if self.bg_scroll_y >= 0:
                self.current_bg = self.next_bg
                self.bg_index = (self.bg_index + 1) % len(self.bg_list)
                self.next_bg = self.bg_list[self.bg_index]
                self.bg_scroll_y = -HEIGHT

            self.map.move()
            self.drawText(f"模式:挑战[{self.difficulty_name}]",5,20)
            self.drawText(f"击杀:{self.boss_killed}/{self.boss_max}",280,20)
            if self.players:
                hp = self.players.sprites()[0].hp
                self.drawText(f"血量:{hp}", 5, HEIGHT-50)
            self.draw_boss_health_bar()
            btn_p = Button(self.screen, "暂停", WIDTH-115, HEIGHT-55,105,45,(255,90,0),(255,140,0))
            btn_p.draw()

            for e in pygame.event.get():
                if e.type == QUIT: exit()
                if btn_p.is_clicked(e): self.is_paused=True

            if self.boss_killed >= self.boss_max:
                self.game_win=True
                continue
            if not self.boss:
                self.spawn_new_boss()

            if self.players and not self.is_game_over:
                p = self.players.sprites()[0]
                hits = pygame.sprite.spritecollide(p, boss_bullets, True)
                if hits:
                    p.hp -=1
                    if self.sound_on: self.sound.playBombSound()
                    if p.hp <=0:
                        self.show_gameover_img = True
                        self.gameover_start_time = pygame.time.get_ticks()
                for b in HeroPlane.bullets:
                    hit_p = pygame.sprite.spritecollide(b,self.prop_group,True)
                    for pr in hit_p:
                        if self.boss and pygame.sprite.collide_rect(pr,self.boss):
                            self.boss.hp -= pr.damage
                            if self.boss.hp <=0:
                                self.boss.die_animate()
                                self.boss = None
                                self.boss_killed +=1
                        b.kill()
                    if self.boss and pygame.sprite.collide_rect(b,self.boss):
                        self.boss.hp -=1
                        b.kill()
                        if self.boss.hp <=0:
                            self.boss.die_animate()
                            self.boss = None
                            self.boss_killed +=1

            self.prop_group.update()
            self.players.update()
            if self.boss: self.boss.update()
            boss_bullets.update()
            HeroPlane.bullets.update()
            self.players.draw(self.screen)
            self.prop_group.draw(self.screen)
            if self.boss: self.screen.blit(self.boss.image, self.boss.rect)
            boss_bullets.draw(self.screen)
            HeroPlane.bullets.draw(self.screen)
            self.player_bomb.draw()
            self.enemy_bomb.draw()
            pygame.display.update()
            time.sleep(0.03)

class ChallengeBoss(pygame.sprite.Sprite):
    def __init__(self, screen, hp, fire_rate, bullet_speed, bullet_count):
        super().__init__()
        self.screen = screen
        self.w = 220
        self.h = 130
        self.max_hp = hp
        self.hp = hp
        try:
            self.idle_img = pygame.transform.scale(pygame.image.load("./feiji/enemy2.png").convert_alpha(), (self.w,self.h))
        except:
            self.idle_img = pygame.Surface((self.w,self.h))
            self.idle_img.fill((255,0,100))
        self.down_frames = []
        for path in [f"./feiji/enemy2_down{i}.png" for i in range(1,7)]:
            try:
                self.down_frames.append(pygame.transform.scale(pygame.image.load(path).convert_alpha(), (self.w,self.h)))
            except:
                self.down_frames.append(pygame.Surface((self.w,self.h)))
        self.image = self.idle_img
        self.rect = self.image.get_rect(centerx=WIDTH//2, y=60)
        self.hp = hp
        self.fire_rate = fire_rate
        self.bullet_speed = bullet_speed
        self.bullet_count = bullet_count
        self.timer = 0
        self.phase = 0
        self.is_dying = False
        self.frame_idx = 0
        self.frame_timer = 0
        self.frame_delay =7

    def update(self):
        if self.is_dying:
            self.frame_timer +=1
            if self.frame_timer >= self.frame_delay:
                self.frame_timer=0
                self.frame_idx +=1
                if self.frame_idx >= len(self.down_frames):
                    self.kill()
                    return
                self.image = self.down_frames[self.frame_idx]
            return
        self.timer +=1
        if self.timer >= self.fire_rate:
            self.fire()
            self.timer=0
            self.phase=(self.phase+1)%4

    def die_animate(self):
        if not self.is_dying:
            self.is_dying=True
            self.frame_idx=0
            self.frame_timer=0

    def fire(self):
        [self.circle_fire, self.spread_fire, self.double_ring, self.full_cover][self.phase]()
    def circle_fire(self):
        n=self.bullet_count
        for i in range(n):
            a=math.radians(360/n*i)
            b=ChallengeBullet(self.screen,self.rect.centerx,self.rect.centery,math.cos(a)*self.bullet_speed, math.sin(a)*self.bullet_speed)
            boss_bullets.add(b)
    def spread_fire(self):
        for dx in [-60,-30,0,30,60]:
            b=ChallengeBullet(self.screen,self.rect.centerx+dx,self.rect.bottom,0,self.bullet_speed)
            boss_bullets.add(b)
    def double_ring(self):
        for i in range(self.bullet_count):
            a=math.radians(360/self.bullet_count*i)
            b=ChallengeBullet(self.screen,self.rect.centerx,self.rect.centery,math.cos(a)*self.bullet_speed,math.sin(a)*self.bullet_speed)
            boss_bullets.add(b)
        for i in range(self.bullet_count):
            a=math.radians(360/self.bullet_count*i+15)
            b=ChallengeBullet(self.screen,self.rect.centerx,self.rect.centery,math.cos(a)*self.bullet_speed*0.8,math.sin(a)*self.bullet_speed*0.8)
            boss_bullets.add(b)
    def full_cover(self):
        for dx in range(-80,81,20):
            b=ChallengeBullet(self.screen,self.rect.centerx+dx,self.rect.bottom,0,self.bullet_speed)
            boss_bullets.add(b)
        for i in range(12):
            a=math.radians(360/12*i)
            b=ChallengeBullet(self.screen,self.rect.centerx,self.rect.centery,math.cos(a)*self.bullet_speed,math.sin(a)*self.bullet_speed)
            boss_bullets.add(b)

class ChallengeBullet(pygame.sprite.Sprite):
    def __init__(self,screen,x,y,sx,sy):
        super().__init__()
        self.screen=screen
        self.image=pygame.Surface((9,9))
        self.image.fill((255,50,50))
        self.rect=self.image.get_rect(center=(x,y))
        self.speed_x=sx
        self.speed_y=sy
    def update(self):
        self.rect.x += self.speed_x
        self.rect.y += self.speed_y
        if self.rect.top>HEIGHT or self.rect.bottom<0 or self.rect.left<0 or self.rect.right>WIDTH:
            self.kill()

def start_challenge(screen, difficulty_name, music_on, sound_on):
    game = ChallengeScene(screen, difficulty_name, music_on, sound_on)
    game.run()