/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Ruben Van Boxem
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 **/

/*
 * Window abstraction.
 */

#ifndef SKUI_GUI_WINDOW_H
#define SKUI_GUI_WINDOW_H

#include "gui/icon.h++"

#include <core/bitflag.h++>
#include <core/proxy_property.h++>
#include <core/string.h++>
#include <core/trackable.h++>

#include <graphics/context.h++>
#include <graphics/position.h++>
#include <graphics/size.h++>

#include <condition_variable>
#include <thread>
#include <vector>

namespace skui
{
  namespace gui
  {
    namespace implementation
    {
      class platform_handle;
      struct platform_handle_deleter
      {
        void operator()(platform_handle*) const;
      };
      using platform_handle_ptr = std::unique_ptr<platform_handle, platform_handle_deleter>;
      platform_handle_ptr create_handle();
    }

    enum class window_flag
    {
      none,
      opengl, // use OpenGL to render window content
      exit_on_close, // Quit application when last window with this flag set closes
    };
    using window_flags = core::bitflag<window_flag>;
    using namespace core::bitflag_operators;

    enum class window_state
    {
      hidden,
      windowed,
      maximized,
      fullscreen_maximized,
      minimized
    };

    class window : public core::trackable
    {
    public:
      using window_list = std::vector<window*>;

      static const window_flags default_flags;

      window(graphics::pixel_position position = {0, 0},
             graphics::pixel_size initial_size = {800, 600},
             window_flags flags = default_flags);
      virtual ~window();

      void show();
      void hide();
      void close();

      void repaint();

      /// Properties
      graphics::pixel_size size;
      graphics::pixel_size maximum_size;
      graphics::pixel_size minimum_size;

      graphics::pixel_position position;

      core::property<gui::icon> icon;
      core::proxy_property<core::string> title;

      /// Signals
      // Window manager
      core::signal<> closed;
      core::property<window_state> state;

    private:
      // temporary drawing of a dummy image
      void draw();

      implementation::platform_handle_ptr native_handle;
      void initialize_and_execute_platform_loop();
      void choose_visual(implementation::platform_handle& handle);
      void setup_window(implementation::platform_handle& handle);
      void setup_graphics_backend(implementation::platform_handle& handle);
      void create_graphics_context();
      void execute_event_loop();

      void update_geometry();
      void swap_buffers();

      void set_title(const core::string& title);
      core::string get_title() const;

      static window_list& windows();

      std::unique_ptr<graphics::context> graphics_context;

      std::mutex handle_mutex;
      std::condition_variable handle_condition_variable;
      std::thread thread;
      window_flags flags;
    };
  }
}

#endif
