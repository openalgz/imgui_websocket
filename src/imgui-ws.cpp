/*! \file imgui-ws.cpp
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#include "imgui-ws/imgui-ws.h"

#include <atomic>
#include <condition_variable>
#include <cstring>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <thread>

#include "common.h"
#include "imgui-ws/imgui-draw-data-compressor.h"
#include "incppect/incppect.h"

// not using ssl
using incppect = incpp::Incppect<false>;

struct ImGuiWS::Impl
{
   struct Events
   {
      std::deque<Event> data;

      std::mutex mutex;
      std::condition_variable cv;

      void push(Event&& event)
      {
         std::lock_guard<std::mutex> lock(mutex);
         data.push_back(std::move(event));
         cv.notify_one();
      }
   };

   struct Data
   {
      std::map<int, TextureId> textureIdMap;
      std::map<TextureId, Texture> textures;

      ImDrawDataCompressor::Interface::DrawLists draw_lists{};
      ImDrawDataCompressor::Interface::DrawListsDiff draw_lists_diff{};
   };

   std::atomic<int32_t> nConnected = 0;

   std::future<void> worker;
   mutable std::shared_mutex mutex;

   Data dataWrite;
   Data dataRead;

   Events events;

   incpp::Incppect<false> incpp;

   THandler handlerConnect;
   THandler handlerDisconnect;

   std::unique_ptr<ImDrawDataCompressor::Interface> compressorDrawData{new ImDrawDataCompressor::XorRlePerDrawListWithVtxOffset()};
};

ImGuiWS::ImGuiWS() : m_impl(new Impl()) {}

ImGuiWS::~ImGuiWS()
{
   if (m_impl->worker.valid()) {
      m_impl->incpp.stop();
      m_impl->worker.get();
   }

   {
      // TODO: This is need to avoid memory corruption upon deletion, but I have no idea why.
      std::vector<char> texture_data_dummy;
      std::swap(texture_data, texture_data_dummy);

      std::vector<char> draw_list_data_dummy;
      std::swap(draw_list_data, draw_list_data_dummy);
   }
}

bool ImGuiWS::addVar(const TPath& path, TGetter&& getter) { return m_impl->incpp.var(path, std::move(getter)); }

void ImGuiWS::addResource(const std::string& url, const std::string& content)
{
   m_impl->incpp.set_resource(url, content);
}

bool ImGuiWS::init(int32_t port, std::string http_root, std::vector<std::string> resources)
{
   m_impl->incpp.var("/my_id/{}", [](const auto& idxs) {
      static int32_t id;
      id = idxs[0];
      return incpp::view(id);
   });

   // number of textures available
   m_impl->incpp.var("/imgui/n_textures", [this](const auto&) {
      std::shared_lock lock(m_impl->mutex);

      return incpp::view(m_impl->dataRead.textures.size());
   });

   // texture ids
   m_impl->incpp.var("/imgui/texture_id/{}", [this](const auto& idxs) {
      std::shared_lock lock(m_impl->mutex);

      if (m_impl->dataRead.textureIdMap.find(idxs[0]) == m_impl->dataRead.textureIdMap.end()) {
         return std::string_view{};
      }

      return incpp::view(m_impl->dataRead.textureIdMap[idxs[0]]);
   });

   // texture revision
   m_impl->incpp.var("/imgui/texture_revision/{}", [this](const auto& idxs) {
      std::shared_lock lock(m_impl->mutex);

      if (m_impl->dataRead.textures.find(idxs[0]) == m_impl->dataRead.textures.end()) {
         return std::string_view{};
      }

      return incpp::view(m_impl->dataRead.textures[idxs[0]].revision);
   });

   // get texture by id
   m_impl->incpp.var("/imgui/texture_data/{}", [this](const auto& idxs) {
      {
         std::shared_lock lock(m_impl->mutex);
         if (m_impl->dataRead.textures.find(idxs[0]) == m_impl->dataRead.textures.end()) {
            return std::string_view{0, 0};
         }

         texture_data = m_impl->dataRead.textures[idxs[0]].data;
      }

      return std::string_view{texture_data.data(), texture_data.size()};
   });

   // get imgui's draw data
   m_impl->incpp.var("/imgui/n_draw_lists", [this](const auto&) {
      std::shared_lock lock(m_impl->mutex);

      return incpp::view(m_impl->dataRead.draw_lists.size());
   });

   m_impl->incpp.var("/imgui/draw_list/{}", [this](const auto& idxs) {
      {
         std::shared_lock lock(m_impl->mutex);

         auto& drawLists = m_impl->dataRead.draw_lists;

         if (idxs[0] >= (int)drawLists.size()) {
            return std::string_view{nullptr, 0};
         }

         draw_list_data = drawLists[idxs[0]];
      }

      return std::string_view{draw_list_data.data(), draw_list_data.size()};
   });

   m_impl->incpp.handler = [&](int clientId, incppect::event etype, std::string_view data) {
      Event event;

      event.clientId = clientId;

      using enum incppect::event;
      switch (etype) {
      case connect: {
         ++m_impl->nConnected;
         event.type = Event::Connected;
         std::stringstream ss;
         {
            int a = data[0];
            if (a < 0) a += 256;
            ss << a << ".";
         }
         {
            int a = data[1];
            if (a < 0) a += 256;
            ss << a << ".";
         }
         {
            int a = data[2];
            if (a < 0) a += 256;
            ss << a << ".";
         }
         {
            int a = data[3];
            if (a < 0) a += 256;
            ss << a;
         }
         event.ip = ss.str();
         if (m_impl->handlerConnect) {
            m_impl->handlerConnect();
         }
      } break;
      case disconnect: {
         --m_impl->nConnected;
         event.type = Event::Disconnected;
         if (m_impl->handlerDisconnect) {
            m_impl->handlerDisconnect();
         }
      } break;
      case custom: {
         std::stringstream ss;
         ss << data;

         int type = -1;
         ss >> type;

         // printf("Received event %d '%s'\n", type, ss.str().c_str());
         switch (type) {
         case 0: {
            // mouse move
            event.type = Event::MouseMove;
            ss >> event.mouse_x >> event.mouse_y;
            // printf("    mouse %g %g\n", event.mouse_x, event.mouse_y);
         } break;
         case 1: {
            // mouse down
            event.type = Event::MouseDown;
            ss >> event.mouse_but >> event.mouse_x >> event.mouse_y;
            // printf("    mouse %d down\n", event.mouse_but);
         } break;
         case 2: {
            // mouse up
            event.type = Event::MouseUp;
            ss >> event.mouse_but >> event.mouse_x >> event.mouse_y;
            // printf("    mouse %d up\n", event.mouse_but);
         } break;
         case 3: {
            // mouse wheel
            event.type = Event::MouseWheel;
            ss >> event.wheel_x >> event.wheel_y;
         } break;
         case 4: {
            // key press
            event.type = Event::KeyPress;
            ss >> event.key;
         } break;
         case 5: {
            // key down
            event.type = Event::KeyDown;
            ss >> event.key;
         } break;
         case 6: {
            // key up
            event.type = Event::KeyUp;
            ss >> event.key;
         } break;
         case 7: {
            // resize
            event.type = Event::Resize;
            ss >> event.client_width >> event.client_height;
         } break;
         case 8: {
            // take control
            event.type = Event::TakeControl;
         } break;
         default: {
            printf("Unknown input received from client: id = %d, type = %d\n", clientId, type);
            return;
         } break;
         };
      } break;
      };

      m_impl->events.push(std::move(event));
   };

   resources.push_back("imgui-ws.js");
   m_impl->incpp.set_resource("/imgui-ws.js", kImGuiWS_js);

   // start the http/websocket server
   incpp::Parameters parameters{.port = port,
                                .max_payload = 1024 * 1024,
                                .t_last_req_timeout_ms = -1,
                                .http_root = std::move(http_root),
                                .resources = std::move(resources),
                                .ssl_key = "key.pem",
                                .ssl_cert = "cert.pem"};

   m_impl->worker = m_impl->incpp.run_async(parameters);

   return true;
}

bool ImGuiWS::setTexture(TextureId textureId, Texture::Type textureType, int32_t width, int32_t height,
                         const char* data)
{
   int bpp = 1; // bytes per pixel
   switch (textureType) {
   case Texture::Type::Alpha8:
      bpp = 1;
      break;
   case Texture::Type::Gray8:
      bpp = 1;
      break;
   case Texture::Type::RGB24:
      bpp = 3;
      break;
   case Texture::Type::RGBA32:
      bpp = 4;
      break;
   };

   if (m_impl->dataWrite.textures.find(textureId) == m_impl->dataWrite.textures.end()) {
      m_impl->dataWrite.textures[textureId].revision = 0;
      m_impl->dataWrite.textures[textureId].data.clear();
      m_impl->dataWrite.textureIdMap.clear();

      int idx = 0;
      for (const auto& t : m_impl->dataWrite.textures) {
         m_impl->dataWrite.textureIdMap[idx++] = t.first;
      }
   }

   m_impl->dataWrite.textures[textureId].revision++;
   m_impl->dataWrite.textures[textureId].data.resize(sizeof(TextureId) + sizeof(Texture::Type) + 3 * sizeof(int32_t) +
                                                     bpp * width * height);

   int revision = m_impl->dataWrite.textures[textureId].revision;

   size_t offset = 0;
   std::memcpy(m_impl->dataWrite.textures[textureId].data.data() + offset, &textureId, sizeof(textureId));
   offset += sizeof(textureId);
   std::memcpy(m_impl->dataWrite.textures[textureId].data.data() + offset, &textureType, sizeof(textureType));
   offset += sizeof(textureType);
   std::memcpy(m_impl->dataWrite.textures[textureId].data.data() + offset, &width, sizeof(width));
   offset += sizeof(width);
   std::memcpy(m_impl->dataWrite.textures[textureId].data.data() + offset, &height, sizeof(height));
   offset += sizeof(height);
   std::memcpy(m_impl->dataWrite.textures[textureId].data.data() + offset, &revision, sizeof(revision));
   offset += sizeof(revision);
   std::memcpy(m_impl->dataWrite.textures[textureId].data.data() + offset, data, bpp * width * height);

   {
      std::unique_lock lock(m_impl->mutex);
      m_impl->dataRead.textures[textureId] = m_impl->dataWrite.textures[textureId];
      m_impl->dataRead.textureIdMap = m_impl->dataWrite.textureIdMap;
   }

   return true;
}

bool ImGuiWS::init(int32_t port, std::string http_root, std::vector<std::string> resources, THandler&& handlerConnect,
                   THandler&& handlerDisconnect)
{
   m_impl->handlerConnect = std::move(handlerConnect);
   m_impl->handlerDisconnect = std::move(handlerDisconnect);

   return init(port, std::move(http_root), std::move(resources));
}

bool ImGuiWS::setDrawData(const ImDrawData* drawData)
{
   bool result = m_impl->compressorDrawData->setDrawData(drawData);

   // make the draw lists available to incppect clients
   {
      std::unique_lock lock(m_impl->mutex);

      std::swap(m_impl->dataRead.draw_lists, m_impl->compressorDrawData->draw_lists_cur);
      std::swap(m_impl->dataRead.draw_lists_diff, m_impl->compressorDrawData->draw_lists_diff);
   }

   return result;
}

int32_t ImGuiWS::nConnected() const { return m_impl->nConnected; }

std::deque<ImGuiWS::Event> ImGuiWS::takeEvents()
{
   std::lock_guard<std::mutex> lock(m_impl->events.mutex);
   auto res = m_impl->events.data;
   m_impl->events.data.clear();
   return res;
}
