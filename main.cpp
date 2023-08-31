#include <iostream>
#include <vector>

#include <fmt/format.h>

#include "raylib.h"
#include "raymath.h"

constexpr double sol = 25902;

struct Body {
  const char *name = nullptr;
  double mass = 1.0;
  double radius = 1.0;
  Vector3 c{0, 0, 0};
  Vector3 v{0, 0, 0};
  Vector3 a{0, 0, 0};
  Color color{255, 255, 255, 255};
  double r(const Body &b) const {
    return sqrt(pow(c.x - b.c.x, 2.0) + pow(c.y - b.c.y, 2.0) +
                pow(c.z - b.c.z, 2.0));
  }
  double r3(const Body &b) const {
    auto rb = r(b);
    return 1 / (rb * rb * rb);
  }
  double mm() const {
    double vel = v.x * v.x + v.y * v.y + v.z * v.z;
    return mass / sqrt(1 - vel / (sol * sol));
  }
};

Vector3 operator+(const Vector3 &a, const Vector3 &b) {
  return Vector3Add(a, b);
}

Vector3 operator*(const Vector3 &a, const Vector3 &b) {
  return Vector3Multiply(a, b);
}

Vector3 operator/(const Vector3 &a, float f) {
  return Vector3Scale(a, 1.0f / f);
}

Vector3 operator*(const Vector3 &a, float f) { return Vector3Scale(a, f); }

Vector3 operator-(const Vector3 &a, const Vector3 &b) {
  return Vector3Subtract(a, b);
}

Vector3 operator-(float f, const Vector3 &a) { return Vector3{f, f, f} - a; }

void init(std::vector<Body> &v, double dt) {
  for (int i = 0; i < v.size(); ++i) {
    Vector3 sum{0, 0, 0};
    for (int j = 0; j < v.size(); ++j) {
      if (i != j) {
        auto r = v[i].r3(v[j]);
        sum = sum + ((v[i].c - v[j].c) * -1 * v[j].mm() * r);
      }
    }
    v[i].a = sum;
  }
  for (Body &x : v) {
    x.v = x.v + x.a * 0.5 * dt;
  }
}

Vector3 Vector3Sqrt(const Vector3 &v) {
  return Vector3{sqrt(v.x), sqrt(v.y), sqrt(v.z)};
}

void cycle(std::vector<Body> &v, double dt, double steps) {
  for (int i = 0; i < steps; ++i) {
    for (Body &b : v) {
      b.c = b.c + (b.v * dt) * Vector3Sqrt(1.0 - (b.v / sol) * (b.v / sol));
    }
    for (int i = 0; i < v.size(); ++i) {
      Vector3 sum{0, 0, 0};
      for (int j = 0; j < v.size(); ++j) {
        if (i != j) {
          auto r = v[i].r3(v[j]);
          sum = sum + (v[i].c - v[j].c) * -2.975e-3 * v[j].mm() * r;
        }
      }
      v[i].a = sum;
    }
    for (Body &b : v) {
      b.v = b.v + b.a * dt;
    }
  }
}

int main(int argc, char *argv[]) {
  int win_w = 1000;
  int win_h = 1000;
  int fps = 60;

  std::vector<Body> bodies;
  bodies.emplace_back("Sun", 332837.0, 1.3, Vector3{0, 0, 0}, Vector3{0, 0, 0},
                      Vector3{0, 0, 0}, WHITE);
  bodies.emplace_back("Earth", 1.0, 0.012, Vector3{0, 150, 0},
                      Vector3{2.568, 0, 0}, Vector3{0, 0, 0}, BLUE);
  bodies.emplace_back("Jupiter", 317.83, 0.14, Vector3{0, 780, 0},
                      Vector3{1.13, 0, 0}, Vector3{0, 0, 0}, ORANGE);
  bodies.emplace_back("Saturn", 95.159, 0.116, Vector3{0, 1437, 0},
                      Vector3{0.837, 0, 0}, Vector3{0, 0, 0}, YELLOW);
  bodies.emplace_back("Uranus", 14.535, 0.051, Vector3{0, 2884, 0},
                      Vector3{0.588, 0, 0}, Vector3{0, 0, 0}, GREEN);
  bodies.emplace_back("Neptune", 17.147, 0.05, Vector3{0, 4515, 0},
                      Vector3{0.469, 0, 0}, Vector3{0, 0, 0}, SKYBLUE);
  bodies.emplace_back("Venus", 0.815, 0.012, Vector3{0, 108, 0},
                      Vector3{-3.025, 0, 0}, Vector3{0, 0, 0}, BEIGE);
  bodies.emplace_back("Mars", 0.107, 0.00678, Vector3{0, 228, 0},
                      Vector3{2.084, 0, 0}, Vector3{0, 0, 0}, RED);
  bodies.emplace_back("Mercury", 0.0562, 0.00488, Vector3{0, 70, 0},
                      Vector3{4.1, 0, 0}, Vector3{0, 0, 0}, LIGHTGRAY);
  bodies.emplace_back("Moon", 0.0203, 0.003, Vector3{0, 150.3, 0},
                      Vector3{2.56 + 8.83e-2, 0, 0}, Vector3{0, 0, 0},
                      LIGHTGRAY);
  bodies.emplace_back("Ceres", 0.0004, 0.001, Vector3{0, 414, 0},
                      Vector3{1.545, 0, 0}, Vector3{0, 0, 0}, LIGHTGRAY);
  bodies.emplace_back("Titan", 0.0660, 0.0051, Vector3{0, 1437 + 1.22, 0},
                      Vector3{0.837 + 0.48, 0, 0}, Vector3{0, 0, 0}, GOLD);
  bodies.emplace_back("Ganymede", 0.0704, 0.0053, Vector3{0, 780 + 1.07, 0},
                      Vector3{1.13 + 0.93, 0, 0}, Vector3{0, 0, 0}, LIGHTGRAY);
  bodies.emplace_back("Comet", 3.68e-11, 0.0001, Vector3{0, 88, 0},
                      Vector3{4.7, 0, 0}, Vector3{0, 0, 0}, WHITE);

  InitWindow(win_w, win_h, "Gravity2");
  SetTargetFPS(fps);

  Camera2D camera;
  camera.offset = Vector2{win_w / 2.0f, win_h / 2.0f};
  camera.rotation = 0;
  camera.zoom = 1;

  int body_number = 0;
  int steps_per_frame = 100;
  double precision = 0.001;
  init(bodies, precision);

  while (!WindowShouldClose()) {
    cycle(bodies, precision, steps_per_frame);
    auto wheel_move = GetMouseWheelMove();
    camera.zoom += 0.1 * wheel_move;
    camera.zoom = Clamp(camera.zoom, 0, 10);
    if (IsKeyPressed(KEY_LEFT)) {
      body_number = (body_number - 1) % bodies.size();
    }
    if (IsKeyPressed(KEY_RIGHT)) {
      body_number = (body_number + 1) % bodies.size();
    }
    if (IsKeyPressed(KEY_UP)) {
      steps_per_frame = Clamp(steps_per_frame * 1.1, 1, 1000000);
    }
    if (IsKeyPressed(KEY_DOWN)) {
      steps_per_frame = Clamp(steps_per_frame * 0.9, 1, 1000000);
    }
    BeginDrawing();
    ClearBackground(BLACK);
    camera.target = Vector2{bodies[body_number].c.x, bodies[body_number].c.y};
    BeginMode2D(camera);
    for (const auto &b : bodies) {
      auto r = std::max(4.0 / camera.zoom,
                        std::min(64.0 / camera.zoom, b.radius * 100));
      DrawCircle(b.c.x, b.c.y, r, b.color);
      DrawText(b.name, b.c.x, b.c.y, 20, WHITE);
    }
    EndMode2D();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}