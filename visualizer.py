
import pygame, subprocess, random

WIDTH, HEIGHT = 800, 500
ENGINE = "engine/sort_engine.exe"

# Get sorting steps from C++ program
def get_steps(arr):
    result = subprocess.run([ENGINE] + [str(x) for x in arr], capture_output=True, text=True)
    steps = []
    for line in result.stdout.strip().split("\n"):
        nums = list(map(int, line.split()))
        steps.append((nums[0], nums[1], nums[2:]))  # (highlight_index, swapped, array)
    return steps

def count_swaps(steps, idx):
    total = 0
    for i in range(idx + 1):
        total += steps[i][1]
    return total
pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Bubble Sort — C++ + Pygame")
font = pygame.font.SysFont("Consolas", 16)
num_font = pygame.font.SysFont("Consolas", 11)
title_font = pygame.font.SysFont("Consolas", 22, bold=True)
slider_font = pygame.font.SysFont("Consolas", 14)
clock = pygame.time.Clock()

# ---------- STATES ----------
STATE_SELECT = 0
STATE_SORT = 1
state = STATE_SELECT

# Slider settings
N = 20
MIN_N, MAX_N = 10, 50
slider_x, slider_w = 200, 400
slider_y = 250
dragging = False

running = True
while running:
    dt = clock.tick(60)
    for e in pygame.event.get():
        if e.type == pygame.QUIT:
            running = False
        if e.type == pygame.KEYDOWN:
            if e.key == pygame.K_ESCAPE:
                running = False
            if e.key == pygame.K_SPACE:
                playing = not playing
            if e.key == pygame.K_RIGHT and not playing:
                if step_idx < len(steps) - 1:
                    step_idx += 1
            if e.key == pygame.K_LEFT and not playing:
                if step_idx > 0:
                    step_idx -= 1
            if e.key == pygame.K_UP:
                speed = max(MIN_SPEED, speed - 20)
            if e.key == pygame.K_DOWN:
                speed = min(MAX_SPEED, speed + 20)
            if e.key == pygame.K_r:
                random.shuffle(arr)
                steps = get_steps(arr)
                step_idx = 0
                playing = False
                timer = 0

    # Advance
    if playing and step_idx < len(steps) - 1:
        timer += dt
        while timer >= speed and step_idx < len(steps) - 1:
            step_idx += 1
            timer -= speed

    # Draw
    screen.fill((20, 20, 30))
    hi, cur = steps[step_idx]
    done = (hi == -1)

    # Title
    screen.blit(font.render("Bubble Sort — C++ Engine + Pygame", True, (100, 140, 255)), (200, 10))

    # Bars with numbers
    bar_w = (WIDTH - 40) // N
    for i, val in enumerate(cur):
        bar_h = int(val / N * (HEIGHT - 120))
        x = 20 + i * bar_w
        y = HEIGHT - 60 - bar_h
        if done:
            color = (0, 200, 100)
        elif i == hi or i == hi + 1:
            color = (255, 80, 80)
        else:
            color = (70, 130, 230)
        pygame.draw.rect(screen, color, (x, y, bar_w - 2, bar_h))

        # Number on bar
        num_text = num_font.render(str(val), True, (255, 255, 255))
        tx = x + (bar_w - 2) // 2 - num_text.get_width() // 2
        ty = y + 2 if bar_h > 16 else y - 14
        screen.blit(num_text, (tx, ty))

    # Status bar
    state = "PAUSED" if not playing else "PLAYING"
    if done:
        state = "DONE"
    speed_label = f"Speed: {speed}ms"
    step_label = f"Step: {step_idx}/{len(steps)-1}"
    info = f"{state}  |  {step_label}  |  {speed_label}"
    screen.blit(font.render(info, True, (180, 180, 200)), (20, HEIGHT - 30))

    controls = "SPC:Play/Pause  \u2190\u2192:Step  \u2191\u2193:Speed  R:Reset  ESC:Quit"
    screen.blit(font.render(controls, True, (100, 100, 130)), (200, HEIGHT - 50))

    pygame.display.flip()

pygame.quit()
