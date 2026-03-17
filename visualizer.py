"""
Bubble Sort Visualizer — C++ engine + Pygame
Run: python visualizer.py
"""
import pygame, subprocess, random

WIDTH, HEIGHT = 800, 500
N = 30
ENGINE = "engine/sort_engine.exe"

# Get sorting steps from C++ program
def get_steps(arr):
    result = subprocess.run([ENGINE] + [str(x) for x in arr], capture_output=True, text=True)
    steps = []
    for line in result.stdout.strip().split("\n"):
        nums = list(map(int, line.split()))
        steps.append((nums[0], nums[1:]))  # (highlight_index, array)
    return steps

pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Bubble Sort — C++ + Pygame")
font = pygame.font.SysFont("Consolas", 16)
clock = pygame.time.Clock()

# Init
arr = list(range(1, N + 1))
random.shuffle(arr)
steps = get_steps(arr)
step_idx = 0
playing = False
timer = 0

running = True
while running:
    dt = clock.tick(60)
    for e in pygame.event.get():
        if e.type == pygame.QUIT:
            running = False
        if e.type == pygame.KEYDOWN:
            if e.key == pygame.K_ESCAPE: running = False
            if e.key == pygame.K_SPACE: playing = not playing
            if e.key == pygame.K_r:
                random.shuffle(arr)
                steps = get_steps(arr)
                step_idx = 0
                playing = False

    # Advance
    if playing and step_idx < len(steps) - 1:
        timer += dt
        while timer >= 100 and step_idx < len(steps) - 1:
            step_idx += 1
            timer -= 100

    # Draw
    screen.fill((20, 20, 30))
    hi, cur = steps[step_idx]
    done = (hi == -1)

    screen.blit(font.render("Bubble Sort — C++ Engine + Pygame", True, (100, 140, 255)), (200, 10))

    bar_w = (WIDTH - 40) // N
    for i, val in enumerate(cur):
        bar_h = int(val / N * (HEIGHT - 100))
        x = 20 + i * bar_w
        y = HEIGHT - 50 - bar_h
        if done: color = (0, 200, 100)
        elif i == hi or i == hi + 1: color = (255, 80, 80)
        else: color = (70, 130, 230)
        pygame.draw.rect(screen, color, (x, y, bar_w - 2, bar_h))

    screen.blit(font.render("SPACE: Play/Pause  R: Reset  ESC: Quit", True, (100, 100, 130)), (220, HEIGHT - 30))
    pygame.display.flip()

pygame.quit()
