import os
import random

def gen_svg(path: str, w: int, h: int, cx: float, cy: float, R: float, obstacles: list[tuple[float, float, float]]):
    with open(path, "w", encoding="utf-8") as f:
        f.write(f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}">\n')
        f.write(f'  <circle cx="{cx:.2f}" cy="{cy:.2f}" r="{R:.2f}" />\n')
        for (ox, oy, r) in obstacles:
            f.write(f'  <circle cx="{ox:.2f}" cy="{oy:.2f}" r="{r:.2f}" />\n')
        f.write('</svg>\n')

def main():
    out_dir = "test_svgs"
    os.makedirs(out_dir, exist_ok=True)

    random.seed(1234)

    for i in range(20):
        w = random.choice([700, 800, 900, 1000])
        h = random.choice([700, 800, 900])

        cx = random.uniform(w * 0.35, w * 0.65)
        cy = random.uniform(h * 0.35, h * 0.65)

        R = random.uniform(min(w, h) * 0.25, min(w, h) * 0.42)

        n = random.randint(2, 5)
        obstacles = []
        for _ in range(n):
            r = random.uniform(R * 0.08, R * 0.18)
            a = random.uniform(0.0, 6.28318530718)
            d = random.uniform(0.0, R - r - 10.0)
            ox = cx + d * (random.uniform(0.8, 1.0)) * (1.0 if random.random() < 0.5 else -1.0) * abs(random.uniform(0.0, 1.0))
            oy = cy + d * (random.uniform(0.8, 1.0)) * (1.0 if random.random() < 0.5 else -1.0) * abs(random.uniform(0.0, 1.0))

            # Keep inside arena (simple clamp)
            vx = ox - cx
            vy = oy - cy
            dist = (vx * vx + vy * vy) ** 0.5
            if dist > (R - r - 5.0) and dist > 1e-6:
                scale = (R - r - 5.0) / dist
                ox = cx + vx * scale
                oy = cy + vy * scale

            obstacles.append((ox, oy, r))

        path = os.path.join(out_dir, f"arena_{i:02d}.svg")
        gen_svg(path, w, h, cx, cy, R, obstacles)

if __name__ == "__main__":
    main()
