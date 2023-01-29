#include "entt/entity/fwd.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <entt/entt.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <spdlog/spdlog.h>

// https://dev.to/noah11012/using-sdl2-in-c-and-with-c-too-1l72

// todo: refactor to use entt::resource_cache
class ResourceManager {
public:
  ResourceManager(std::shared_ptr<SDL_Window> window,
                  std::shared_ptr<SDL_Renderer> renderer)
      : m_renderer(renderer), m_window(window) {}

  std::shared_ptr<SDL_Texture> loadTexture(const std::string &path) {
    SDL_Surface *surface = SDL_LoadBMP(path.c_str());
    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(m_renderer.get(), surface);

    if (!texture) {
      spdlog::error("Failed to convert surface into a texture");
      spdlog::error("SDL2 Error: {}", SDL_GetError());

      throw 41;
    }

    SDL_FreeSurface(surface);

    return std::shared_ptr<SDL_Texture>(
        texture, [](SDL_Texture *texture) { SDL_DestroyTexture(texture); });
  }

private:
  std::shared_ptr<SDL_Window> m_window;
  std::shared_ptr<SDL_Renderer> m_renderer;
};

class IScene {
  friend class Game;

public:
  virtual void setup(const std::shared_ptr<ResourceManager> resourceManager){};

protected:
  entt::registry m_registry = entt::registry();
};

class Game {
public:
  Game() { initialize(); };

  void run(std::unique_ptr<IScene> scene) {
    bool keep_window_open = true;

    enterScene(std::move(scene));

    while (keep_window_open) {
      SDL_Event e;
      while (SDL_PollEvent(&e) > 0) {
        switch (e.type) {
        case SDL_QUIT:
          keep_window_open = false;
          break;
        }
        SDL_UpdateWindowSurface(m_window.get());

        // call render and update systems on registry
        auto registry = &m_currentScene->m_registry;
      }
    }
  }

  void enterScene(std::unique_ptr<IScene> scene) {
    spdlog::info("Transitioning Scene");
    m_currentScene = std::move(scene);
    m_currentScene->setup(m_resourceManager);
  }

private:
  std::shared_ptr<SDL_Window> m_window;
  std::shared_ptr<SDL_Renderer> m_renderer;
  std::shared_ptr<ResourceManager> m_resourceManager;

  std::unique_ptr<IScene> m_currentScene;

  void initialize() {
    spdlog::info("Initializing SDL2");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      spdlog::error("Failed to initialize the SDL2 library");
      throw 21;
    }
    spdlog::info("Creating SDL2 Window");
    m_window = std::shared_ptr<SDL_Window>(
        SDL_CreateWindow("boucy hoverboats", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, 1600, 900, 0),
        [](SDL_Window *window) { SDL_DestroyWindow(window); });

    if (!m_window) {
      spdlog::error("Failed to create window");
      throw 22;
    }

    spdlog::info("Creating SDL2 Renderer");
    m_renderer = std::shared_ptr<SDL_Renderer>(
        SDL_CreateRenderer(m_window.get(), -1, SDL_RENDERER_ACCELERATED),
        [](SDL_Renderer *renderer) { SDL_DestroyRenderer(renderer); });

    if (!m_renderer) {
      spdlog::error("Failed to create window");
      throw 23;
    }

    spdlog::info("Initializing ResourceManager");
    m_resourceManager = std::make_shared<ResourceManager>(m_window, m_renderer);
  }
};

struct Transform2D {
  float x;
  float y;
  float rotation;
};

struct Velocity {
  float dx;
  float dy;
};

struct Texture2D {
  std::shared_ptr<SDL_Texture> texture;
};

void addRenderingSystem(const entt::registry &registry) {
  auto renderables = registry.view<const Transform2D, const Texture2D>();
  renderables.each([](const Transform2D &position, const Texture2D &texture) {
    spdlog::debug("RENDERING");
  });
};

class MainMenuScene : public IScene {
  void setup(const std::shared_ptr<ResourceManager> resourceManager) override {
    spdlog::info("Initializing Main Menu");

    addRenderingSystem(m_registry);

    auto entity = m_registry.create();
    m_registry.emplace<Transform2D>(entity, 0.f, 0.f);
    m_registry.emplace<Texture2D>(
        entity, resourceManager->loadTexture("hoverboat.bmp"));
  };
};

int main(int argc, char **argv) {
  try {
    Game().run(std::make_unique<MainMenuScene>());
  } catch (int code) {
    spdlog::error("Unexpected error occured: Error Code {}", code);
  }
}
