#include "SDL_surface.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <entt/entt.hpp>
#include <iostream>
#include <memory>
#include <spdlog/spdlog.h>

// https://dev.to/noah11012/using-sdl2-in-c-and-with-c-too-1l72

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
      std::cout << "Failed to convert surface into a texture\n";
      std::cout << "SDL2 Error: " << SDL_GetError() << "\n";

      throw -1;
    }

    SDL_FreeSurface(surface);

    return std::shared_ptr<SDL_Texture>(
        texture, [](SDL_Texture* texture){SDL_DestroyTexture(texture);});
  }

private:
  std::shared_ptr<SDL_Window> m_window;
  std::shared_ptr<SDL_Renderer> m_renderer;
};

class IScene {
  virtual void initialize(const ResourceManager &resourceManager);
};

class Game {
public:
  Game() { initialize(); };

  void run(std::unique_ptr<IScene> scene) {
    bool keep_window_open = true;
    while (keep_window_open) {
      SDL_Event e;
      while (SDL_PollEvent(&e) > 0) {
        switch (e.type) {
        case SDL_QUIT:
          keep_window_open = false;
          break;
        }
        SDL_UpdateWindowSurface(m_window.get());
      }
    }
  }

private:
  std::shared_ptr<SDL_Window> m_window;
  std::shared_ptr<SDL_Renderer> m_renderer;
  std::shared_ptr<ResourceManager> m_resourceManager;

  std::shared_ptr<entt::registry> m_registry;

  void initialize() {
    spdlog::info("Initializing SDL2");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      spdlog::error("Failed to initialize the SDL2 library");
      throw -1;
    }
    spdlog::info("Creating SDL2 Window");
    m_window = std::shared_ptr<SDL_Window>(
        SDL_CreateWindow("boucy hoverboats", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, 1600, 900, 0),
        [](SDL_Window *window) { SDL_DestroyWindow(window); });

    if (!m_window) {
      spdlog::error("Failed to create window");
      throw -1;
    }

    spdlog::info("Creating SDL2 Renderer");
    m_renderer = std::shared_ptr<SDL_Renderer>(
        SDL_CreateRenderer(m_window.get(), -1, SDL_RENDERER_ACCELERATED),
        [](SDL_Renderer *renderer) { SDL_DestroyRenderer(renderer); });

    if (!m_renderer) {
      spdlog::error("Failed to create window");
      throw -1;
    }

    spdlog::info("Initializing ResourceManager");
    m_resourceManager = std::make_shared<ResourceManager>(m_window, m_renderer);
  }
};

class MainMenuScene : IScene {
  void initialize(const ResourceManager &resourceManager) override {
    spdlog::info("Initializing Main Menu");
  };
};

int main(int argc, char **argv) { Game().run(std::move(nullptr)); }
