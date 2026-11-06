import pygame
import random
import math
from pygame.constants import *
from config import BG_SIZE

WIDTH, HEIGHT = BG_SIZE

# 全局子弹组
enemy_bullets = pygame.sprite.Group()
elite_bullets = pygame.sprite.Group()
boss_bullets = pygame.sprite.Group()


# ===================== 普通敌机 EnemyPlane：素材enemy0 + down1~down4 =====================
class EnemyPlane(pygame.sprite.Sprite):
    def __init__(self, screen, speed):
        super().__init__()
        self.screen = screen
        self.speed = speed
        self.hp = 1
        self.w, self.h = 52, 32

        # 普通飞机待机图 enemy0.png
        try:
            self.idle_img = pygame.transform.scale(pygame.image.load("./feiji/enemy0.png").convert_alpha(), (self.w, self.h))
        except:
            self.idle_img = pygame.Surface((self.w, self.h))
            self.idle_img.fill((255, 0, 0))

        # 普通机死亡4帧：enemy0_down1~4
        self.down_frames = []
        down_list = [
            "./feiji/enemy0_down1.png",
            "./feiji/enemy0_down2.png",
            "./feiji/enemy0_down3.png",
            "./feiji/enemy0_down4.png"
        ]
        for path in down_list:
            try:
                frm = pygame.transform.scale(pygame.image.load(path).convert_alpha(), (self.w, self.h))
                self.down_frames.append(frm)
            except:
                tmp = pygame.Surface((self.w, self.h))
                tmp.fill((220,220,220))
                self.down_frames.append(tmp)

        self.image = self.idle_img
        self.rect = self.image.get_rect()
        self.rect.x = random.randint(0, WIDTH - self.rect.width)
        self.rect.y = -self.rect.height

        # 爆炸动画参数
        self.is_dying = False
        self.frame_idx = 0
        self.frame_timer = 0
        self.frame_delay = 6

        self.shoot_timer = 0
        self.shoot_interval = 60

    def update(self):
        # 死亡爆炸动画
        if self.is_dying:
            self.frame_timer += 1
            if self.frame_timer >= self.frame_delay:
                self.frame_idx += 1
                self.frame_timer = 0
                if self.frame_idx >= len(self.down_frames):
                    self.kill()
                    return
                self.image = self.down_frames[self.frame_idx]
            return

        # 正常下落+射击
        self.rect.y += self.speed
        if self.rect.top > HEIGHT:
            self.kill()
        self.shoot_timer += 1
        if self.shoot_timer >= self.shoot_interval:
            self.shoot()
            self.shoot_timer = 0

    def shoot(self):
        bullet = EnemyBullet(self.screen, self.rect.centerx, self.rect.bottom)
        enemy_bullets.add(bullet)

    def die_animate(self):
        if not self.is_dying:
            self.is_dying = True
            self.frame_idx = 0
            self.frame_timer = 0

    @staticmethod
    def clear_bullets():
        enemy_bullets.empty()


# ===================== 精英敌机 EliteEnemy：素材enemy1 + down1~down4 =====================
class EliteEnemy(pygame.sprite.Sprite):
    def __init__(self, screen, speed):
        super().__init__()
        self.screen = screen
        self.speed = speed
        self.hp = 3
        self.w, self.h = 78, 48

        # 精英待机 enemy1.png
        try:
            self.idle_img = pygame.transform.scale(pygame.image.load("./feiji/enemy1.png").convert_alpha(), (self.w, self.h))
        except:
            self.idle_img = pygame.Surface((self.w, self.h))
            self.idle_img.fill((0, 255, 255))

        # 精英爆炸4帧 enemy1_down1~4
        self.down_frames = []
        down_name_list = [
            "./feiji/enemy1_down1.png",
            "./feiji/enemy1_down2.png",
            "./feiji/enemy1_down3.png",
            "./feiji/enemy1_down4.png"
        ]
        for path in down_name_list:
            try:
                frame = pygame.transform.scale(pygame.image.load(path).convert_alpha(), (self.w, self.h))
                self.down_frames.append(frame)
            except:
                temp = pygame.Surface((self.w, self.h))
                temp.fill((220,220,220))
                self.down_frames.append(temp)

        self.image = self.idle_img
        self.rect = self.image.get_rect()
        self.rect.x = random.randint(0, WIDTH - self.rect.width)
        self.rect.y = -self.rect.height

        self.is_dying = False
        self.frame_index = 0
        self.frame_timer = 0
        self.frame_delay = 6

        self.shoot_timer = 0
        self.shoot_interval = 18

    def update(self):
        if self.is_dying:
            self.frame_timer += 1
            if self.frame_timer >= self.frame_delay:
                self.frame_index += 1
                self.frame_timer = 0
                if self.frame_index >= len(self.down_frames):
                    self.kill()
                    return
                self.image = self.down_frames[self.frame_index]
            return

        self.rect.y += self.speed
        if self.rect.top > HEIGHT:
            self.kill()
        self.shoot_timer += 1
        if self.shoot_timer >= self.shoot_interval:
            self.shoot()
            self.shoot_timer = 0

    def shoot(self):
        bullet = EliteBullet(self.screen, self.rect.centerx, self.rect.bottom)
        elite_bullets.add(bullet)

    def die_animate(self):
        if not self.is_dying:
            self.is_dying = True
            self.frame_index = 0
            self.frame_timer = 0

    @staticmethod
    def clear_bullets():
        elite_bullets.empty()


# ===================== BOSS敌机 BossEnemy：素材enemy2 + down1~down6（6帧爆炸） =====================
class BossEnemy(pygame.sprite.Sprite):
    def __init__(self, screen):
        super().__init__()
        self.screen = screen
        self.w, self.h = 220, 130
        self.max_hp = 50
        self.hp = 50

        # BOSS原图 enemy2.png
        try:
            self.idle_img = pygame.transform.scale(pygame.image.load("./feiji/enemy2.png").convert_alpha(), (self.w, self.h))
        except:
            self.idle_img = pygame.Surface((self.w, self.h))
            self.idle_img.fill((255, 0, 255))

        # BOSS死亡6帧 enemy2_down1~enemy2_down6
        self.down_frames = []
        boss_down = [
            "./feiji/enemy2_down1.png",
            "./feiji/enemy2_down2.png",
            "./feiji/enemy2_down3.png",
            "./feiji/enemy2_down4.png",
            "./feiji/enemy2_down5.png",
            "./feiji/enemy2_down6.png"
        ]
        for path in boss_down:
            try:
                frm = pygame.transform.scale(pygame.image.load(path).convert_alpha(), (self.w, self.h))
                self.down_frames.append(frm)
            except:
                tmp = pygame.Surface((self.w, self.h))
                tmp.fill((120,0,120))
                self.down_frames.append(tmp)

        self.image = self.idle_img
        self.rect = self.image.get_rect()
        self.rect.centerx = WIDTH // 2
        self.rect.y = 50

        # BOSS爆炸动画
        self.is_dying = False
        self.frame_idx = 0
        self.frame_timer = 0
        self.frame_delay = 7

        self.shoot_timer = 0
        self.shoot_interval = 60
        self.normal_shoot_count = 0

    def update(self):
        # BOSS血量耗尽播放爆炸
        if self.is_dying:
            self.frame_timer += 1
            if self.frame_timer >= self.frame_delay:
                self.frame_idx += 1
                self.frame_timer = 0
                if self.frame_idx >= len(self.down_frames):
                    self.kill()
                    return
                self.image = self.down_frames[self.frame_idx]
            return

        self.shoot_timer += 1
        if self.shoot_timer >= self.shoot_interval:
            self.attack()
            self.shoot_timer = 0

    def attack(self):
        patterns = [self.normal_shoot, self.spread_shoot, self.circle_shoot, self.wave_shoot, self.aimed_shoot]
        pattern_idx = (self.shoot_timer // self.shoot_interval) % len(patterns)
        patterns[pattern_idx]()

    def normal_shoot(self):
        """普通发射：向前发射5颗子弹"""
        offsets = [-45, -22, 0, 22, 45]
        for dx in offsets:
            b = BossBullet(self.screen, self.rect.centerx + dx, self.rect.bottom)
            boss_bullets.add(b)

    def spread_shoot(self):
        """散射发射：扇形发射9颗子弹"""
        for i in range(-4, 5):
            angle = math.radians(90 + i * 15)  # 扇形角度
            b = BossBullet(self.screen, self.rect.centerx, self.rect.centery + 50)
            b.speed_x = math.cos(angle) * 7
            b.speed_y = math.sin(angle) * 7
            boss_bullets.add(b)

    def circle_shoot(self):
        """圆形发射：向四周发射16颗子弹"""
        count = 16
        for i in range(count):
            angle = math.radians(360 / count * i)
            b = BossBullet(self.screen, self.rect.centerx, self.rect.centery)
            b.speed_x = math.cos(angle) * 6
            b.speed_y = math.sin(angle) * 6
            boss_bullets.add(b)

    def wave_shoot(self):
        """波浪发射：波浪式发射多颗子弹"""
        for i in range(-3, 4):
            b = BossBullet(self.screen, self.rect.centerx + i * 35, self.rect.bottom)
            b.speed_x = i * 0.8  # 轻微左右偏移
            boss_bullets.add(b)

    def aimed_shoot(self):
        """追踪发射：向玩家方向发射3颗快速子弹"""
        try:
            player = pygame.sprite.spritecollideany(HeroPlane, [])
            if player:
                dx = player.rect.centerx - self.rect.centerx
                dy = player.rect.centery - self.rect.centery
                dist = max(1, math.sqrt(dx*dx + dy*dy))
                for i in range(-1, 2):
                    b = BossBullet(self.screen, self.rect.centerx + i * 30, self.rect.bottom)
                    b.speed_x = dx / dist * 10 + i * 1.5
                    b.speed_y = abs(dy / dist) * 10
                    boss_bullets.add(b)
            else:
                # 如果没有玩家信息，发射普通子弹
                self.normal_shoot()
        except:
            self.normal_shoot()

    def die_animate(self):
        if not self.is_dying:
            self.is_dying = True
            self.frame_idx = 0
            self.frame_timer = 0

    @staticmethod
    def clear_bullets():
        boss_bullets.empty()


# ===================== 普通敌机子弹 =====================
class EnemyBullet(pygame.sprite.Sprite):
    def __init__(self, screen, x, y):
        super().__init__()
        self.screen = screen
        self.image = pygame.Surface((5, 10))
        self.image.fill((255, 255, 0))
        self.rect = self.image.get_rect()
        self.rect.centerx = x
        self.rect.top = y
        self.speed = 8

    def update(self):
        self.rect.y += self.speed
        if self.rect.top > HEIGHT:
            self.kill()


# ===================== 精英敌机子弹 =====================
class EliteBullet(pygame.sprite.Sprite):
    def __init__(self, screen, x, y):
        super().__init__()
        self.screen = screen
        self.image = pygame.Surface((6, 12))
        self.image.fill((0, 255, 255))
        self.rect = self.image.get_rect()
        self.rect.centerx = x
        self.rect.top = y
        self.speed = 10

    def update(self):
        self.rect.y += self.speed
        if self.rect.top > HEIGHT:
            self.kill()


# ===================== BOSS子弹 =====================
class BossBullet(pygame.sprite.Sprite):
    def __init__(self, screen, x, y):
        super().__init__()
        self.screen = screen
        self.image = pygame.Surface((8, 8))
        self.image.fill((255, 0, 100))
        self.rect = self.image.get_rect()
        self.rect.centerx = x
        self.rect.centery = y
        self.speed_x = 0
        self.speed_y = 6

    def update(self):
        self.rect.x += self.speed_x
        self.rect.y += self.speed_y
        if self.rect.top > HEIGHT or self.rect.left < 0 or self.rect.right > WIDTH:
            self.kill()