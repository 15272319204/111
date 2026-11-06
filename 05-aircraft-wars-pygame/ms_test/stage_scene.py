import pygame
import random
import time
from pygame.constants import *
from config import BG_SIZE, CREATE_ENEMY_ID
from player import HeroPlane
from enemy import EnemyPlane, EliteEnemy, BossEnemy, enemy_bullets, elite_bullets, boss_bullets
from tools import GameBackground, Bomb, GameSound
from button import Button
import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from prop import Prop

class StageGameScene:
    def __init__(self, screen, level, music_on, sound_on):
        self.screen = screen
        self.level = level
        self.music_on = music_on
        self.sound_on = sound_on
        self.enemy_speed = 6 + level * 0.35
        self.spawn_ms = max(350, 1200 - level*8)

        self.bg_list = []
        for i in range(1, 6):
            try:
                img = pygame.image.load(f"./feiji/img_bg_level_{i}.jpg").convert()
                img = pygame.transform.scale(img, BG_SIZE)
                self.bg_list.append(img)
            except:
                self.bg_list.append(pygame.Surface(BG_SIZE))

        self.bg_index = 0
        self.current_bg = self.bg_list[0]
        self.next_bg = self.bg_list[1]
        self.bg_scroll_y = -BG_SIZE[1]

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
        self.score = 0
        self.target_score = 1200 + level*600
        self.players = pygame.sprite.Group()
        self.enemys = pygame.sprite.Group()
        self.elites = pygame.sprite.Group()
        self.boss = None
        self.player_bomb = Bomb(screen, 'player')
        self.enemy_bomb = Bomb(screen, 'enemy')
        self.sound = GameSound()
        self.is_paused = False
        self.is_game_over = False
        self.is_stage_clear = False
        self.running = True
        self.high_score = self.load_high_score()

        # 道具状态
        self.shield_active = False
        self.shield_timer = 0
        self.speed_boost = False
        self.speed_boost_timer = 0
        self.double_score = False
        self.double_score_timer = 0

    def reset(self):
        EnemyPlane.clear_bullets()
        EliteEnemy.clear_bullets()
        BossEnemy.clear_bullets()
        HeroPlane.clear_bullets()
        self.players.empty()
        self.enemys.empty()
        self.elites.empty()
        self.boss = None
        self.prop_group.empty()
        self.score = 0
        self.is_paused = False
        self.is_game_over = False
        self.is_stage_clear = False
        self.bg_index = 0
        self.current_bg = self.bg_list[0]
        self.next_bg = self.bg_list[1]
        self.bg_scroll_y = -BG_SIZE[1]
        self.show_gameover_img = False
        pygame.time.set_timer(CREATE_ENEMY_ID, int(self.spawn_ms))
        if self.music_on:
            self.sound.playBackgroundMusic()
        self.players.add(HeroPlane(self.screen))

        # 重置道具状态
        self.shield_active = False
        self.shield_timer = 0
        self.speed_boost = False
        self.speed_boost_timer = 0
        self.double_score = False
        self.double_score_timer = 0

    def drawText(self, text, x, y, size=30, color=(255,255,255)):
        try:
            font = pygame.font.Font('./feiji/baddf.ttf', size)
        except:
            font = pygame.font.SysFont(None, size)
        shadow = font.render(text, True, (0, 0, 0))
        self.screen.blit(shadow, (x + 2, y + 2))
        self.screen.blit(font.render(text, True, color), (x, y))

    def draw_boss_health_bar(self):
        if not self.boss:
            return
        bar_width = 300
        bar_height = 20
        bar_x = (BG_SIZE[0] - bar_width) // 2
        bar_y = 50
        pygame.draw.rect(self.screen, (50, 50, 50), (bar_x, bar_y, bar_width, bar_height), border_radius=10)
        hp_percent = max(0, self.boss.hp / self.boss.max_hp)
        fill_width = int(bar_width * hp_percent)
        gradient_color = (255 - int(255 * hp_percent), int(255 * hp_percent), 50)
        pygame.draw.rect(self.screen, gradient_color, (bar_x, bar_y, fill_width, bar_height), border_radius=10)
        pygame.draw.rect(self.screen, (255, 255, 255), (bar_x, bar_y, bar_width, bar_height), border_radius=10, width=2)
        try:
            font = pygame.font.Font('./feiji/baddf.ttf', 18)
        except:
            font = pygame.font.SysFont(None, 18)
        hp_text = f"Boss HP: {self.boss.hp}/{self.boss.max_hp}"
        text_surf = font.render(hp_text, True, (255, 255, 255))
        text_rect = text_surf.get_rect(center=(BG_SIZE[0] // 2, bar_y + bar_height // 2))
        self.screen.blit(text_surf, text_rect)

    def load_high_score(self):
        try:
            with open('./feiji/highscore.txt', 'r') as f:
                return int(f.read())
        except:
            return 0

    def save_high_score(self):
        if self.score > self.high_score:
            self.high_score = self.score
            try:
                with open('./feiji/highscore.txt', 'w') as f:
                    f.write(str(self.high_score))
            except:
                pass

    def spawn_enemy(self):
        if not self.is_paused and not self.is_game_over and not self.is_stage_clear:
            self.enemys.add(EnemyPlane(self.screen, self.enemy_speed))

    def get_elite_count(self):
        return len(list(self.elites.sprites()))

    def spawn_elite(self):
        if not self.is_paused and not self.is_game_over and not self.is_stage_clear and self.get_elite_count() <= 2 and random.randint(1, 100) == 1:
            self.elites.add(EliteEnemy(self.screen, self.enemy_speed))

    def spawn_boss(self):
        if not self.is_paused and not self.is_game_over and not self.is_stage_clear and self.score >= self.target_score - 1000 and self.boss == None and random.randint(1, 100) == 1:
            self.boss = BossEnemy(self.screen)

    def show_pause(self):
        btn_cont = Button(self.screen, "继续", 120, 180, 240, 65, (0,170,0), (0,210,0))
        btn_rest = Button(self.screen, "重新开始", 120, 260, 240, 65, (170,170,0), (210,210,0))
        btn_back = Button(self.screen, "返回选关", 120, 340, 240, 65, (170,0,0), (210,0,0))
        btn_cont.draw()
        btn_rest.draw()
        btn_back.draw()
        for e in pygame.event.get():
            if e.type == QUIT: pygame.quit();exit()
            if btn_cont.is_clicked(e): self.is_paused=False
            if btn_rest.is_clicked(e): self.reset()
            if btn_back.is_clicked(e): self.running=False
        pygame.display.update()

    def show_gameover(self):
        self.save_high_score()
        s = pygame.Surface(BG_SIZE)
        s.set_alpha(200)
        s.fill((0,0,0))
        self.screen.blit(s, (0,0))
        self.drawText(f"第{self.level}关失败", BG_SIZE[0]//2-90,200,55,(255,0,0))
        self.drawText(f"得分:{self.score}/{self.target_score}",BG_SIZE[0]//2-120,300,38,(255,255,0))
        self.drawText(f"最高分:{self.high_score}", BG_SIZE[0]//2-120,350,30,(200,200,200))
        btn_rest = Button(self.screen, "重新闯关",120,400,240,55,(0,170,0),(0,210,0))
        btn_back = Button(self.screen, "返回选关",120,470,240,55,(180,0,0),(220,0,0))
        btn_rest.draw()
        btn_back.draw()
        for e in pygame.event.get():
            if e.type == QUIT: pygame.quit();exit()
            if btn_rest.is_clicked(e): self.reset()
            if btn_back.is_clicked(e): self.running=False
        pygame.display.update()

    def show_clear(self):
        self.save_high_score()
        s = pygame.Surface(BG_SIZE)
        s.set_alpha(180)
        s.fill((0,0,0))
        self.screen.blit(s, (0,0))
        self.drawText(f"第{self.level}关通关！",BG_SIZE[0]//2-105,200,55,(0,255,0))
        self.drawText(f"得分:{self.score}", BG_SIZE[0]//2-80,270,35,(255,255,0))
        self.drawText(f"最高分:{self.high_score}", BG_SIZE[0]//2-90,310,30,(200,200,200))
        btn_next = Button(self.screen, "下一关",120,360,240,55,(0,170,170),(0,210,210))
        btn_back = Button(self.screen, "返回选关",120,430,240,55,(180,0,0),(220,0,0))
        btn_next.draw()
        btn_back.draw()
        for e in pygame.event.get():
            if e.type == QUIT: pygame.quit();exit()
            if btn_next.is_clicked(e):
                self.level +=1
                self.__init__(self.screen,self.level,self.music_on,self.sound_on)
                self.reset()
            if btn_back.is_clicked(e): self.running=False
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
            if self.is_stage_clear:
                self.show_clear()
                continue

            # 道具生成 - 使用新的4种道具类型
            self.prop_tick +=1
            if self.prop_tick >= 180:  # 每3秒生成一个道具
                self.prop_group.add(Prop(self.screen))
                self.prop_tick = 0

            self.screen.blit(self.current_bg, (0,0))
            self.screen.blit(self.next_bg, (0, self.bg_scroll_y))
            self.bg_scroll_y += 2
            if self.bg_scroll_y >= 0:
                self.current_bg = self.next_bg
                self.bg_index = (self.bg_index + 1) % len(self.bg_list)
                self.next_bg = self.bg_list[self.bg_index]
                self.bg_scroll_y = -BG_SIZE[1]

            self.map.move()
            self.drawText(f"关卡:{self.level} 分数:{self.score}/{self.target_score}",5,20)
            hp = self.players.sprites()[0].hp if self.players else 0
            self.drawText(f"血量:{hp}",5,BG_SIZE[1]-50)

            # 显示道具状态
            y_offset = 50
            if self.shield_active:
                self.drawText(f"护盾: {self.shield_timer//60+1}s", BG_SIZE[0]-150, y_offset, 18, (0, 255, 255))
                y_offset += 25
            if self.speed_boost:
                self.drawText(f"加速: {self.speed_boost_timer//60+1}s", BG_SIZE[0]-150, y_offset, 18, (255, 165, 0))
                y_offset += 25
            if self.double_score:
                self.drawText(f"双倍: {self.double_score_timer//60+1}s", BG_SIZE[0]-150, y_offset, 18, (255, 215, 0))

            btn_pause = Button(self.screen,"暂停",BG_SIZE[0]-115,BG_SIZE[1]-55,105,45,(255,90,0),(255,130,0))
            btn_pause.draw()
            self.draw_boss_health_bar()
            self.drawText(f"最高分:{self.high_score}", BG_SIZE[0]-180, 30, 20, (200,200,200))

            # 道具图例说明
            legend_x = BG_SIZE[0] - 180
            legend_y = 60
            legend_spacing = 28

            # 青色 - 护盾
            pygame.draw.circle(self.screen, (0, 255, 255), (legend_x + 12, legend_y + 10), 10)
            self.drawText("护盾", legend_x + 28, legend_y, 16, (0, 255, 255))

            # 橙色 - 加速
            legend_y += legend_spacing
            pygame.draw.circle(self.screen, (255, 165, 0), (legend_x + 12, legend_y + 10), 10)
            self.drawText("加速", legend_x + 28, legend_y, 16, (255, 165, 0))

            # 金色 - 双倍分数
            legend_y += legend_spacing
            pygame.draw.circle(self.screen, (255, 215, 0), (legend_x + 12, legend_y + 10), 10)
            self.drawText("双倍分数", legend_x + 28, legend_y, 16, (255, 215, 0))

            # 黄色 - 炸弹（立即触发）
            legend_y += legend_spacing
            pygame.draw.circle(self.screen, (255, 255, 0), (legend_x + 12, legend_y + 10), 10)
            self.drawText("炸弹", legend_x + 28, legend_y, 16, (255, 255, 0))

            for e in pygame.event.get():
                if e.type == QUIT: pygame.quit();exit()
                if e.type == CREATE_ENEMY_ID: self.spawn_enemy()
                if btn_pause.is_clicked(e): self.is_paused=True

            self.spawn_elite()
            self.spawn_boss()

            if self.players:
                p = self.players.sprites()[0]

                # 玩家飞机碰撞道具
                hit_props = pygame.sprite.spritecollide(p, self.prop_group, True)
                for pr in hit_props:
                    if pr.prop_type == 'shield':
                        self.shield_active = True
                        self.shield_timer = 180  # 3秒
                    elif pr.prop_type == 'speed':
                        self.speed_boost = True
                        self.speed_boost_timer = 180  # 3秒
                    elif pr.prop_type == 'score':
                        self.double_score = True
                        self.double_score_timer = 180  # 3秒
                    elif pr.prop_type == 'bomb':
                        # 炸弹立即触发效果
                        enemy_bullets.empty()
                        elite_bullets.empty()
                        boss_bullets.empty()
                        # 消灭所有敌机
                        for enemy in self.enemys:
                            enemy.die_animate()
                        for elite in self.elites:
                            elite.hp -= 10
                            if elite.hp <= 0:
                                elite.die_animate()
                        if self.boss:
                            self.boss.hp -= 20
                            if self.boss.hp <= 0:
                                self.boss.die_animate()
                                self.boss = None
                                self.score += 500
                        if self.sound_on:
                            self.sound.playBombSound()

                # 如果有护盾，子弹不造成伤害
                if not self.shield_active:
                    hit1 = pygame.sprite.spritecollide(p,enemy_bullets,True)
                    hit2 = pygame.sprite.spritecollide(p,elite_bullets,True)
                    hit3 = pygame.sprite.spritecollide(p,boss_bullets,True)
                    if hit1 or hit2 or hit3:
                        p.hp -=1
                        if self.sound_on: self.sound.playBombSound()
                        if p.hp<=0:
                            self.show_gameover_img = True
                            self.gameover_start_time = pygame.time.get_ticks()
                for e in pygame.sprite.spritecollide(p,self.enemys,False): e.die_animate()
                for e in pygame.sprite.spritecollide(p,self.elites,False): e.die_animate()

                for b in HeroPlane.bullets:
                    for e in pygame.sprite.spritecollide(b,self.enemys,False):
                        e.die_animate()
                        score_gain = 100 if self.double_score else 50
                        self.score += score_gain
                        b.kill()
                    for e in pygame.sprite.spritecollide(b,self.elites,False):
                        e.hp -=1
                        b.kill()
                        if e.hp<=0:
                            e.die_animate()
                            score_gain = 200 if self.double_score else 100
                            self.score += score_gain
                    if self.boss and pygame.sprite.collide_rect(b,self.boss):
                        self.boss.hp -=1
                        b.kill()
                        if self.boss.hp<=0:
                            self.boss.die_animate()
                            self.boss=None
                            score_gain = 1000 if self.double_score else 500
                            self.score += score_gain

            if self.score >= self.target_score and not self.boss and not self.is_stage_clear:
                self.is_stage_clear = True

            # 更新道具效果
            if self.shield_timer > 0:
                self.shield_timer -= 1
                if self.shield_timer <= 0:
                    self.shield_active = False
            if self.speed_boost_timer > 0:
                self.speed_boost_timer -= 1
                if self.speed_boost_timer <= 0:
                    self.speed_boost = False
            if self.double_score_timer > 0:
                self.double_score_timer -= 1
                if self.double_score_timer <= 0:
                    self.double_score = False

            self.prop_group.update()
            self.players.update()
            # 如果有加速效果，提高玩家速度
            if self.speed_boost and self.players:
                for p in self.players:
                    p.speed_multiplier = 1.5
            else:
                for p in self.players:
                    p.speed_multiplier = 1.0
            self.enemys.update()
            self.elites.update()
            enemy_bullets.update()
            elite_bullets.update()
            boss_bullets.update()
            self.enemys.draw(self.screen)
            self.elites.draw(self.screen)
            self.prop_group.draw(self.screen)
            enemy_bullets.draw(self.screen)
            elite_bullets.draw(self.screen)
            boss_bullets.draw(self.screen)
            if self.boss:
                self.boss.update()
                self.screen.blit(self.boss.image,self.boss.rect)
            self.players.draw(self.screen)
            pygame.display.update()

def start_stage_game(screen, level, music_on, sound_on):
    game = StageGameScene(screen, level, music_on, sound_on)
    game.run()
