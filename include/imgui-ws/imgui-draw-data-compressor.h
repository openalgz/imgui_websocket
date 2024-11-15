/*! \file imgui-draw-data-compressor.h
 *  \brief Enter description here.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

struct ImDrawData;

namespace ImDrawDataCompressor
{
   struct Interface
   {
      using DrawList = std::vector<char>;
      using DrawLists = std::vector<DrawList>;
      using DrawListDiff = std::vector<char>;
      using DrawListsDiff = std::vector<DrawListDiff>;

      virtual ~Interface() {}

      virtual bool setDrawData(const ::ImDrawData* drawData) = 0;

      uint64_t diffSize() const
      {
         uint64_t res = 0;

         for (const auto& list : draw_lists_diff) {
            res += list.size();
         }

         return res;
      }

      DrawLists draw_lists_cur;
      DrawLists draw_lists_prev;
      DrawListsDiff draw_lists_diff;
   };

   struct XorRlePerDrawList : public Interface
   {
      static constexpr auto kName = "XorRlePerDrawList";

      bool setDrawData(const ::ImDrawData* drawData) override;
   };

   struct XorRlePerDrawListWithVtxOffset : public Interface
   {
      static constexpr auto kName = "XorRlePerDrawListWithVtxOffset";

      bool setDrawData(const ::ImDrawData* drawData) override;
   };
}
